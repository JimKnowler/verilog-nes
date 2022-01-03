#include <numeric>

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/VideoOutputTestBench.h"
using namespace videooutputtestbench;

namespace {
    const int NES_VISIBLE_WIDTH = 256;
    
    class VideoOutput : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(0);
            testBench.reset();
            testBench.trace.clear();
        }

        VideoOutputTestBench testBench;
    };
}

TEST_F(VideoOutput, ShouldConstruct) {
    auto& core = testBench.core();
    
    // testing that test fixture can safely construct and reset

    // should not drive any video output, until the first line buffer has been filled
    EXPECT_EQ(core.o_vga_reset_n, 0);
    EXPECT_EQ(core.o_vga_red, 0);
    EXPECT_EQ(core.o_vga_green, 0);
    EXPECT_EQ(core.o_vga_blue, 0);
}

TEST_F(VideoOutput, ShouldReceiveDataFromFIFO) {
    auto& core = testBench.core();

    core.i_pixel_valid = 1;
    core.i_pixel_rgb = 0xabc;
    testBench.tick();

    // should not drive any video output, until the first line buffer has been filled
    EXPECT_EQ(core.o_vga_reset_n, 0);
    EXPECT_EQ(core.o_vga_red, 0);
    EXPECT_EQ(core.o_vga_green, 0);
    EXPECT_EQ(core.o_vga_blue, 0);
}

TEST_F(VideoOutput, ShouldFillBackBufferAndThenRenderFrontBuffer) {
    auto& core = testBench.core();

    std::function<uint32_t(int)> testPixelValue = [](int i) {
        return (i << 16) + (i << 8) + i;
    };

    // fill back buffer while not rendering VGA
    core.i_pixel_valid = 1;
    core.i_vga_x = 0;
    for (int i=0; i<NES_VISIBLE_WIDTH; i++) {
        core.i_pixel_rgb = testPixelValue(i);
        testBench.tick();
    }

    // render front buffer
    core.i_pixel_valid = 0;
    core.i_pixel_rgb = 0;

    testBench.tick();       // single tick in the horiz sync region

    for (int i=0; i<NES_VISIBLE_WIDTH * 2; i++) {
        core.i_vga_x = i;
        testBench.tick();
    }
    
    TraceBuilder traceBuilder;
    traceBuilder
        .port(i_clk).signal("-_").repeat((NES_VISIBLE_WIDTH * 3) + 1)
        .port(o_debug_vga_visible)
                    .signal("0").repeat(NES_VISIBLE_WIDTH)
                    .signal("1").repeat(1 + (NES_VISIBLE_WIDTH * 2))
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_front)
                    .signal("0").repeat(NES_VISIBLE_WIDTH)
                    .signal("1").repeat((NES_VISIBLE_WIDTH * 2) + 1)
                    .concat().repeatEachStep(2)
        .port(o_vga_reset_n)
                    .signal("0").repeat(NES_VISIBLE_WIDTH)
                    .signal("1").repeat((NES_VISIBLE_WIDTH * 2) + 1)
                    .concat().repeatEachStep(2)
        .port(o_vga_red)
                    .signal({0}).repeat(NES_VISIBLE_WIDTH + 1);

    for (uint32_t i=0; i<NES_VISIBLE_WIDTH; i++) {
        traceBuilder.signal({i, i});
    }

    traceBuilder.concat().repeatEachStep(2);

    Trace expected = traceBuilder;
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
} 

TEST_F(VideoOutput, ShouldFillBackBufferWhileRenderingFrontBufferAndThenRenderNextFrontBuffer) {
    auto& core = testBench.core();

    std::function<uint32_t(int)> testPixelValue1 = [](int i) {
        return (i << 16) + (i << 8) + i;
    };

    std::function<uint32_t(int)> testPixelValue2 = [](int i) {
        int v = (NES_VISIBLE_WIDTH - 1) - i;

        return (v << 16) + (v << 8) + v;
    };

    // fill back buffer while not rendering VGA
    core.i_pixel_valid = 1;
    core.i_vga_x = 0;
    for (int i=0; i<NES_VISIBLE_WIDTH; i++) {
        core.i_pixel_rgb = testPixelValue1(i);
        testBench.tick();
    }

    // end of filling back buffer, needs to tick in horiz sync region to swap line buffers
    core.i_pixel_valid = 0;
    core.i_pixel_rgb = 0;

    testBench.tick();

    // fill back buffer while rendering to VGA
    for (int i=0; i<NES_VISIBLE_WIDTH; i++) {
        core.i_vga_x = 2 * i;
        core.i_pixel_rgb = testPixelValue2(i);
        core.i_pixel_valid = 1;
        testBench.tick();

        core.i_vga_x = (2 * i) + 1;
        core.i_pixel_rgb = 0;
        core.i_pixel_valid = 0;
        testBench.tick();
    }

    // RESET trace - so that we just look at the rendering of the front buffer
    testBench.trace.clear();

    // end of filling back buffer, needs to tick in horiz sync region to swap line buffers
    core.i_pixel_valid = 0;
    core.i_pixel_rgb = 0;
    core.i_vga_x = (2 * (NES_VISIBLE_WIDTH + 1));

    testBench.tick();

    // render front buffer
    for (int i=0; i<NES_VISIBLE_WIDTH * 2; i++) {
        core.i_vga_x = i;

        testBench.tick();
    }
    
    TraceBuilder traceBuilder;
    traceBuilder
        .port(i_clk).signal("-_").repeat((NES_VISIBLE_WIDTH * 2) + 1)
        .port(o_debug_vga_visible)
                    .signal("0")
                    .signal("1").repeat(NES_VISIBLE_WIDTH * 2)
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_write_index)
                    .signal({0}).repeat((NES_VISIBLE_WIDTH * 2) + 1)
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_front)
                    .signal("0").repeat((NES_VISIBLE_WIDTH * 2) + 1)
                    .concat().repeatEachStep(2)
        .port(o_vga_reset_n)
                    .signal("1").repeat((NES_VISIBLE_WIDTH * 2) + 1)
                    .concat().repeatEachStep(2)
        .port(o_vga_red)
                    .signal({0});

    for (uint32_t i=0; i<NES_VISIBLE_WIDTH; i++) {
        uint32_t v = (NES_VISIBLE_WIDTH-1) - i;
        traceBuilder.signal({ v, v});
    }

    traceBuilder.concat().repeatEachStep(2);

    Trace expected = traceBuilder;
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
} 

TEST_F(VideoOutput, ShouldFillBackBufferAndThenRenderFrontBufferMultipleTimes) {
    auto& core = testBench.core();

    std::function<uint32_t(int)> testPixelValue = [](int i) {
        return (i << 16) + (i << 8) + i;
    };

    // fill back buffer while not rendering VGA
    core.i_pixel_valid = 1;
    core.i_vga_x = 0;
    for (int i=0; i<NES_VISIBLE_WIDTH; i++) {
        core.i_pixel_rgb = testPixelValue(i);
        testBench.tick();
    }

    // render front buffer
    core.i_pixel_valid = 0;
    core.i_pixel_rgb = 0;

    testBench.tick();       // single tick in the horiz sync region

    // RESET testbench trace, so that we focus on rendering
    testBench.trace.clear();

    for (int line=0; line<2; line++) {
        for (int i=0; i<NES_VISIBLE_WIDTH * 2; i++) {
            core.i_vga_x = i;
            testBench.tick();
        }

        core.i_vga_x = (NES_VISIBLE_WIDTH+1) * 2;
        testBench.tick();
    }
    
    TraceBuilder traceBuilder;
    traceBuilder
        .port(i_clk).signal("-_").repeat((NES_VISIBLE_WIDTH * 2) + 1)
        .port(o_debug_vga_visible)
                    .signal("1").repeat(NES_VISIBLE_WIDTH * 2)
                    .signal("0")
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_front)
                    .signal("1").repeat((NES_VISIBLE_WIDTH * 2) + 1)
                    .concat().repeatEachStep(2)
        .port(o_vga_reset_n)
                    .signal("1").repeat((NES_VISIBLE_WIDTH * 2) + 1)
                    .concat().repeatEachStep(2)
        .port(o_vga_red);

    for (uint32_t i=0; i<NES_VISIBLE_WIDTH; i++) {
        traceBuilder.signal({i, i});
    }

    traceBuilder
        .signal({0})
        .concat().repeatEachStep(2)
        .allPorts().repeat(2);

    Trace expected = traceBuilder;
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
} 