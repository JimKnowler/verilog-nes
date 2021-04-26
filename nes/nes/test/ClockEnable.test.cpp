#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/ClockEnableTestBench.h"
using namespace clockenabletestbench;

namespace {
    class ClockEnable : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        ClockEnableTestBench testBench;
    };
}

TEST_F(ClockEnable, ShouldConstruct) {
}

TEST_F(ClockEnable, ShouldReset) {
    testBench.reset();

    auto& core = testBench.core();

    ASSERT_EQ(core.o_ce_cpu, 0);
}

TEST_F(ClockEnable, ShouldDivideClock) {
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(6)
        .port(o_ce_cpu).signal("____--").repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
