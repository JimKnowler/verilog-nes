#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUChipEnableTestBench.h"
using namespace ppuchipenabletestbench;

namespace {
    class PPUChipEnable : public ::testing::TestWithParam<int> {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
        }
        
        void TearDown() override {
        }

        PPUChipEnableTestBench testBench;
    };
}

TEST_F(PPUChipEnable, ShouldEnablePPUAtStartOfAddressRange) {
    auto& core = testBench.core();

    core.i_clk_en_cpu = 1;
    core.i_clk_en_ppu = 1;
    core.i_address = 0x2000;
    core.eval();

    EXPECT_EQ(core.o_ce, 1);
}

TEST_F(PPUChipEnable, ShouldEnablePPUAtEndOfAddressRange) {
    auto& core = testBench.core();

    core.i_clk_en_cpu = 1;
    core.i_clk_en_ppu = 1;
    core.i_address = 0x2FFF;
    core.eval();

    EXPECT_EQ(core.o_ce, 1);
}


TEST_F(PPUChipEnable, ShouldNotEnablePPUWhenCPUClkIsDisabled) {
    auto& core = testBench.core();

    core.i_clk_en_cpu = 0;
    core.i_clk_en_ppu = 1;
    core.i_address = 0x2000;
    core.eval();

    EXPECT_EQ(core.o_ce, 0);
}

TEST_F(PPUChipEnable, ShouldNotEnablePPUWhenPPUClkIsDisabled) {
    auto& core = testBench.core();

    core.i_clk_en_cpu = 1;
    core.i_clk_en_ppu = 0;
    core.i_address = 0x2000;
    core.eval();

    EXPECT_EQ(core.o_ce, 0);
}

TEST_F(PPUChipEnable, ShouldNotEnablePPUWhenAddressIsTooHigh) {
    auto& core = testBench.core();

    core.i_clk_en_cpu = 1;
    core.i_clk_en_ppu = 1;
    core.i_address = 0x3000;
    core.eval();

    EXPECT_EQ(core.o_ce, 0);
}

TEST_F(PPUChipEnable, ShouldNotEnablePPUWhenAddressIsTooLow) {
    auto& core = testBench.core();

    core.i_clk_en_cpu = 1;
    core.i_clk_en_ppu = 1;
    core.i_address = 0x1FFF;
    core.eval();
    
    EXPECT_EQ(core.o_ce, 0);
}
