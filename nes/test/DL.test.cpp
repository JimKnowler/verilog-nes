#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/DLTestBench.h"
using namespace dltestbench;

namespace {
    class DL : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        DLTestBench testBench;
    };
}

TEST_F(DL, ShouldConstruct) {
    auto& core = testBench.core();

    EXPECT_EQ(0x00, core.o_data);
}

TEST_F(DL, ShouldReset) {
    testBench.reset();
}

TEST_F(DL, ShouldNotPassThroughDuringPhi1) {
    auto& core = testBench.core();

    core.i_clk = 0;
    core.i_data = 0x42;
    core.eval();
    EXPECT_EQ(0x00, core.o_data);
}

TEST_F(DL, ShouldNotPassThroughDuringPhi2) {
    auto& core = testBench.core();

    core.i_clk = 1;
    core.i_data = 0x42;
    core.eval();
    EXPECT_EQ(0x0, core.o_data);
}

TEST_F(DL, ShouldLoadAtEndOfPhi2) {
    auto& core = testBench.core();

    core.i_clk = 0;
    core.i_data = 0x42;
    core.eval();

    // RISING EDGE
    core.i_clk = 1;
    core.eval();

    // FALLING EDGE
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(0x42, core.o_data);
}
