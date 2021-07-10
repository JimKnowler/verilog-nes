#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUIncrementYTestBench.h"
using namespace ppuincrementytestbench;

namespace {
    uint32_t BIT_OFFSET_FINE_Y = 12;
    uint32_t BIT_OFFSET_COARSE_Y = 5;
    uint32_t BIT_OFFSET_VERTICAL_NAMETABLE = 11;

    class PPUIncrementY : public ::testing::TestWithParam<int> {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        PPUIncrementYTestBench testBench;
    };
}

TEST_F(PPUIncrementY, ShouldIncrementFine) {
    auto& core = testBench.core();

    for (uint16_t y=0; y<7; y++) {
        core.i_v = y << BIT_OFFSET_FINE_Y;
        core.eval();

        EXPECT_EQ(core.o_v, (y+1) << 12);
    }
}

TEST_F(PPUIncrementY, ShouldIncrementCoarse) {
    auto& core = testBench.core();

    core.i_v = 7 << BIT_OFFSET_FINE_Y;
    core.eval();

    EXPECT_EQ(core.o_v, 1 << BIT_OFFSET_COARSE_Y);
}

TEST_F(PPUIncrementY, ShouldIncrementCoarseOverflowtoSecondVerticalNametable) {
    auto& core = testBench.core();

    core.i_v = (7 << BIT_OFFSET_FINE_Y) | (29 << BIT_OFFSET_COARSE_Y);
    core.eval();

    EXPECT_EQ(core.o_v, 1 << BIT_OFFSET_VERTICAL_NAMETABLE);
}

TEST_F(PPUIncrementY, ShouldIncrementCoarseOverflowBackToFirstVerticalNametable) {
    auto& core = testBench.core();

    core.i_v = (7 << BIT_OFFSET_FINE_Y) | (29 << BIT_OFFSET_COARSE_Y) | (1 << BIT_OFFSET_VERTICAL_NAMETABLE);
    core.eval();

    EXPECT_EQ(core.o_v, 0);
}

// TODO: if y == 31, set y  = 0 without toggling vertical nametable
