#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/VGAOutput3x2TestBench.h"
using namespace vgaoutput3x2testbench;

namespace {
    const uint32_t kWidth = 7;
    const uint32_t kHeight = 5;
    const uint32_t kWidthVisible = 3;
    const uint32_t kHeightVisible = 2;
    const uint32_t kFrontPorchX = 1;
    const uint32_t kBackPorchX = 1;
    const uint32_t kFrontPorchY = 1;
    const uint32_t kBackPorchY = 1;
    
    class VGAOutput : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(0);

            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        VGAOutput3x2TestBench testBench;
    };
}

TEST_F(VGAOutput, ShouldConstruct) {
    
}

TEST_F(VGAOutput, ShouldPassThroughColourWhenVisible) {
    auto& core = testBench.core();

    const uint8_t kRed = 0xff;
    const uint8_t kGreen = 0xae;
    const uint8_t kBlue = 0xbc;

    core.i_visible = 1;
    core.i_red = kRed;
    core.i_green = kGreen;
    core.i_blue = kBlue;
    core.eval();

    EXPECT_EQ(kRed, core.o_vga_red);
    EXPECT_EQ(kGreen, core.o_vga_green);
    EXPECT_EQ(kBlue, core.o_vga_blue);
}

TEST_F(VGAOutput, ShouldNotPassThroughColourWhenNotVisible) {
    auto& core = testBench.core();

    const uint8_t kRed = 0xff;
    const uint8_t kGreen = 0xae;
    const uint8_t kBlue = 0xbc;

    core.i_visible = 0;
    core.i_red = kRed;
    core.i_green = kGreen;
    core.i_blue = kBlue;
    core.eval();

    EXPECT_EQ(0, core.o_vga_red);
    EXPECT_EQ(0, core.o_vga_green);
    EXPECT_EQ(0, core.o_vga_blue);
}

TEST_F(VGAOutput, ShouldGenerateSync) {
    const uint32_t kNumTicks = kWidth * kHeight;

    auto& core = testBench.core();

    for (int y = 0; y < kHeight; y++) {
        core.i_y = y;

        for (int x = 0; x < kWidth; x++) {
            core.i_x = x;

            core.i_visible = (x < kWidthVisible) && (y < kHeightVisible);

            testBench.tick();
        }
    }

    Trace expected = TraceBuilder()
        .port(i_clk).signal("-_").repeat(kNumTicks)
        .port(o_vga_hsync)
            .signal("1").repeat(kWidthVisible + kFrontPorchX)
            .signal("0").repeat(kWidth - kWidthVisible - kFrontPorchX - kBackPorchX)
            .signal("1").repeat(kBackPorchX)
            .concat().repeatEachStep(2).repeat(kHeight)
        .port(o_vga_vsync)
            .signal("1").repeat(kHeightVisible + kFrontPorchY)
            .signal("0").repeat(kHeight - kHeightVisible - kFrontPorchY - kBackPorchY)
            .signal("1").repeat(kBackPorchY)
            .concat().repeatEachStep(2 * kWidth);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
