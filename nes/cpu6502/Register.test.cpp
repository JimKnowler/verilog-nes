#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/RegisterTestBench.h"
using namespace registertestbench;

namespace {
    class Register : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        RegisterTestBench testBench;
    };
}

TEST_F(Register, ShouldConstruct) {    
    auto& core = testBench.core();

    EXPECT_EQ(0, core.o_data);
}

TEST_F(Register, ShouldReset) {
    testBench.reset();
    auto& core = testBench.core();

    EXPECT_EQ(0xFF, core.o_data);
}

TEST_F(Register, ShouldLoadOnClockFallingEdge) {
    auto& core = testBench.core();

    core.i_clk = 0;
    core.eval();

    core.i_data = 0xAF;
    core.i_load = 1;
    
    // should not load without clock
    core.eval();
    EXPECT_EQ(0, core.o_data);

    // should not load on rising edge of clock
    core.i_clk = 1;
    core.eval();
    EXPECT_EQ(0, core.o_data);

    // should load on falling edge of clock
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(0xAF, core.o_data);
}

TEST_F(Register, ShouldNotLoadWithoutControlSignal) {
    auto& core = testBench.core();

    core.i_clk = 1;
    core.eval();

    core.i_clk = 0;
    core.i_data = 0xAF;
    core.eval();
    EXPECT_EQ(0, core.o_data);
}
