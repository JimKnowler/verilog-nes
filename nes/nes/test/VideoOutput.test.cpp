#include <numeric>

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/VideoOutputTestBench.h"
using namespace videooutputtestbench;

namespace {
    const int NES_VISIBLE_WIDTH = 255;
    const int VGA_VISIBLE_WIDTH = NES_VISIBLE_WIDTH * 2;
    
    class VideoOutput : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(0);
            testBench.reset();
            testBench.trace.clear();
        }

        VideoOutputTestBench testBench;
    
        void helperSimulateRenderRowVGA() {
            auto& core = testBench.core();

            core.i_pixel_valid = 0;

            for (int i=0; i<VGA_VISIBLE_WIDTH + 1; i++) {
                core.i_vga_x = i;
                testBench.tick();
            }
        }

        void helperSimulateWriteRowNES(std::function<uint32_t(int)> pixelGenerator) {
            auto& core = testBench.core();

            core.i_pixel_valid = 1;
            
            for (int i=0; i<NES_VISIBLE_WIDTH; i++) {
                core.i_pixel_x = i;
                core.i_pixel_rgb = pixelGenerator(i);
                testBench.tick();
            }
        }
    };

    std::function<uint32_t(int)> testPixelValue1 = [](int i) {
        return (i << 16) + (i << 8) + i;
    };

    std::function<uint32_t(int)> testPixelValue2 = [](int i) {
        int v = (NES_VISIBLE_WIDTH - 1) - i;

        return (v << 16) + (v << 8) + v;
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
    core.i_pixel_x = 0;
    core.i_pixel_rgb = 0xabc;
    testBench.tick();

    // should not drive any video output, until the first line buffer has been filled
    EXPECT_EQ(core.o_vga_reset_n, 0);
    EXPECT_EQ(core.o_vga_red, 0);
    EXPECT_EQ(core.o_vga_green, 0);
    EXPECT_EQ(core.o_vga_blue, 0);
}

TEST_F(VideoOutput, ShouldWriteToLineBuffer0) {
    auto& core = testBench.core();

    // fill line buffer 0
    core.i_vga_x = 0;
    helperSimulateWriteRowNES(testPixelValue1);
    
    TraceBuilder traceBuilder;
    traceBuilder
        .port(i_clk).signal("-_").repeat(NES_VISIBLE_WIDTH)
        .port(o_debug_vga_visible)
                    .signal("0").repeat(NES_VISIBLE_WIDTH-1)
                    .signal("1")
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_read)
                    .signal({0}).repeat(NES_VISIBLE_WIDTH)
                    .repeatEachStep(2)
        .port(o_debug_linebuffer_write)
                    .signal({0}).repeat(NES_VISIBLE_WIDTH-1)
                    .signal({1})
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_read_count)
                    .signal("0").repeat(NES_VISIBLE_WIDTH)
                    .repeatEachStep(2)
        .port(o_vga_reset_n)
                    .signal("0").repeat(NES_VISIBLE_WIDTH-1)
                    .signal("1")
                    .concat().repeatEachStep(2)
        .port(o_vga_red)
                    .signal({0}).repeat(NES_VISIBLE_WIDTH)
                    .repeatEachStep(2);

    Trace expected = traceBuilder;
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}


TEST_F(VideoOutput, ShouldWriteToLineBuffer0AndThenReadLineBuffer0Twice) {
    auto& core = testBench.core();

    // fill back buffer while not rendering VGA
    core.i_vga_x = 0;
    helperSimulateWriteRowNES(testPixelValue1);

    testBench.trace.clear();

    for (int repeat=0; repeat<2; repeat++) {
        // each NES linebuffer should be rendered to two VGA rows (i.e. doubling pixels vertically)
        helperSimulateRenderRowVGA();
    }
    
    TraceBuilder traceBuilder;
    traceBuilder
        .port(i_clk).signal("-_").repeat(((VGA_VISIBLE_WIDTH + 1) * 2))
        .port(o_debug_vga_visible)
                    .signal("1").repeat(VGA_VISIBLE_WIDTH)
                    .signal("0")
                    .signal("1").repeat(VGA_VISIBLE_WIDTH)
                    .signal("0")
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_read)
                    .signal({0}).repeat(((VGA_VISIBLE_WIDTH + 1) * 2)-1)
                    .signal({1})
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_write)
                    .signal({1}).repeat(((VGA_VISIBLE_WIDTH + 1) * 2))
                    .concat().repeatEachStep(2)
        .port(o_vga_reset_n)
                    .signal("1").repeat(((VGA_VISIBLE_WIDTH + 1) * 2))
                    .concat().repeatEachStep(2)
        .port(o_vga_red);

    for (int repeat=0; repeat < 2; repeat++) {
        for (uint32_t i=0; i<NES_VISIBLE_WIDTH; i++) {
            traceBuilder.signal({i}).repeat(2);
        }

        traceBuilder.signal({0});
    }

    traceBuilder.concat().repeatEachStep(2);

    Trace expected = traceBuilder;
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
} 

TEST_F(VideoOutput, ShouldWriteToLineBuffer2WhileRenderingLineBuffer1) {
    auto& core = testBench.core();

    // write to line buffer 0
    core.i_vga_x = 0;
    helperSimulateWriteRowNES(testPixelValue1);

    // write to line buffer 1 while rendering linebuffer 0 to VGA
    ASSERT_EQ(core.o_debug_linebuffer_write, 1);
    ASSERT_EQ(core.o_debug_linebuffer_read, 0);

    core.i_vga_x = 0;
    helperSimulateWriteRowNES(testPixelValue2);

    for (int repeat=0; repeat<2; repeat++) {
        // each NES linebuffer should be rendered to two VGA rows (i.e. doubling pixels vertically)
        helperSimulateRenderRowVGA();
    }

    // RESET trace - so that we just look at the rendering of line buffer 1
    testBench.trace.clear();

    // read line buffer 1 while writing to line buffer 2
    ASSERT_EQ(core.o_debug_linebuffer_write, 2);
    ASSERT_EQ(core.o_debug_linebuffer_read, 1);

    helperSimulateRenderRowVGA();
    
    TraceBuilder traceBuilder;
    traceBuilder
        .port(i_clk).signal("-_").repeat(VGA_VISIBLE_WIDTH + 1)
        .port(o_debug_vga_visible)
                    .signal("1").repeat(VGA_VISIBLE_WIDTH)
                    .signal("0")
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_read)
                    .signal({1}).repeat(VGA_VISIBLE_WIDTH + 1)
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_write)
                    .signal({2}).repeat(VGA_VISIBLE_WIDTH + 1)
                    .concat().repeatEachStep(2)
        .port(o_vga_reset_n)
                    .signal("1").repeat(VGA_VISIBLE_WIDTH + 1)
                    .concat().repeatEachStep(2)
        .port(o_vga_red);

    for (uint32_t i=0; i<NES_VISIBLE_WIDTH; i++) {
        uint8_t pixelValueRed = uint8_t(testPixelValue2(i));
        traceBuilder.signal({pixelValueRed}).repeat(2);
    }

    traceBuilder.signal({0});        // non-visible pixel at end of VGA line

    traceBuilder.concat().repeatEachStep(2);

    Trace expected = traceBuilder;
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
} 

TEST_F(VideoOutput, ShouldWriteToLineBuffer2WhileRenderingLineBuffer0) {
    auto& core = testBench.core();

    // fill line buffer 0
    core.i_vga_x = 0;
    helperSimulateWriteRowNES(testPixelValue1);

    // fill line buffer 1 while rendering linebuffer 0 to VGA
    ASSERT_EQ(core.o_debug_linebuffer_write, 1);
    ASSERT_EQ(core.o_debug_linebuffer_read, 0);

    core.i_vga_x = 0;
    helperSimulateWriteRowNES(testPixelValue2);

    // RESET trace - so that we just look at the rendering of line buffer 1
    testBench.trace.clear();

    // read line buffer 1 while writing to line buffer 2
    ASSERT_EQ(core.o_debug_linebuffer_write, 2);
    ASSERT_EQ(core.o_debug_linebuffer_read, 0);

    helperSimulateRenderRowVGA();
    
    TraceBuilder traceBuilder;
    traceBuilder
        .port(i_clk).signal("-_").repeat(VGA_VISIBLE_WIDTH + 1)
        .port(o_debug_vga_visible)
                    .signal("1").repeat(VGA_VISIBLE_WIDTH)
                    .signal("0")
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_read)
                    .signal({0}).repeat(VGA_VISIBLE_WIDTH + 1)
                    .concat().repeatEachStep(2)
        .port(o_debug_linebuffer_write)
                    .signal({2}).repeat(VGA_VISIBLE_WIDTH + 1)
                    .concat().repeatEachStep(2)
        .port(o_vga_reset_n)
                    .signal("1").repeat(VGA_VISIBLE_WIDTH + 1)
                    .concat().repeatEachStep(2)
        .port(o_vga_red);

    for (uint32_t i=0; i<NES_VISIBLE_WIDTH; i++) {
        uint8_t pixelValueRed = uint8_t(testPixelValue1(i));
        traceBuilder.signal({pixelValueRed}).repeat(2);
    }

    traceBuilder.signal({0});        // non-visible pixel at end of VGA line

    traceBuilder.concat().repeatEachStep(2);

    Trace expected = traceBuilder;
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
} 


// todo: wrap around read/write linebuffer indices