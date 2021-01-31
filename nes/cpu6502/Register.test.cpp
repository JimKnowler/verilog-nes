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
            testBench.setClockPolarity(1);
            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        RegisterTestBench testBench;
    };
}

TEST_F(Register, ShouldConstruct) {    
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
    EXPECT_EQ(0xFF, core.o_data);
}

TEST_F(Register, ShouldNotPassThroughWhileLoadSignalIsHigh) {
    auto& core = testBench.core();

    core.i_data = 0xAF;
    core.i_load = 1;
    core.eval();
    EXPECT_EQ(0xFF, core.o_data);
}

TEST_F(Register, ShouldLoadOnFallingEdgeOfClkWhileLoadIsHigh) {
    auto& core = testBench.core();

    core.i_data = 0;
    core.i_load = 0;
    testBench.tick();
    
    core.i_data = 0xAF;
    testBench.tick();

    core.i_load = 1;
    testBench.tick();

    core.i_load = 0;
    core.i_load = 0x12;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_data).signal({0xFF,0xFF,0xAF,0xAF}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
