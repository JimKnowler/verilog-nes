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

TEST_F(Register, ShouldNotPassThroughWhileLoadSignalIsLow) {
    auto& core = testBench.core();

    core.i_data = 0xAF;
    core.i_load = 0;
    core.eval();
    EXPECT_EQ(0, core.o_data);
}

TEST_F(Register, ShouldPassThroughWhileLoadSignalIsHigh) {
auto& core = testBench.core();

    core.i_data = 0xAF;
    core.i_load = 1;
    core.eval();
    EXPECT_EQ(0xAF, core.o_data);
}

TEST_F(Register, ShouldLoadOnFallingEdgeOfLoadSignal) {
    auto& core = testBench.core();

    core.i_data = 0xAF;
    core.i_load = 0;
    core.eval();

    // RISING EDGE
    core.i_load = 1;
    core.eval();
    EXPECT_EQ(0xAF, core.o_data);

    // FALLING EDGE
    core.i_load = 0;
    core.eval();
    EXPECT_EQ(0xAF, core.o_data);
}
