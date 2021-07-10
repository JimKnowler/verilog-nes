#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUIncrementXTestBench.h"
using namespace ppuincrementxtestbench;

namespace {
    class PPUIncrementX : public ::testing::TestWithParam<int> {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        PPUIncrementXTestBench testBench;
    };
}

TEST_F(PPUIncrementX, ShouldIncrementCoarse) {
    auto& core = testBench.core();

    for (uint8_t v=0; v<31; v++) {
        core.i_v = v;
        core.eval();

        EXPECT_EQ(core.o_v, v+1);
    }
}

TEST_F(PPUIncrementX, ShouldIncrementCoarseOverflowToSecondHorizontalNametable) {
    auto& core = testBench.core();

    core.i_v = 31;
    core.eval();

    EXPECT_EQ(core.o_v, 0x0400);
}

TEST_F(PPUIncrementX, ShouldIncrementCoarseOverflowBackToFirstHorizontalNametable) {
    auto& core = testBench.core();

    core.i_v = 0x0400 + 31;
    core.eval();

    EXPECT_EQ(core.o_v, 0x0000);
}

