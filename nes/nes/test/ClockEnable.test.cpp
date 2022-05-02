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

            testBench.core().i_ce = 1;
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
        .port(o_ce_cpu).signal("____--").repeat(2)
        .port(o_ce_ppu).signal("------").repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ClockEnable, ShouldHandleClockDisable) {
    testBench.core().i_ce = 0;

    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(6)
        .port(o_ce_cpu).signal("______").repeat(2)
        .port(o_ce_ppu).signal("______").repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ClockEnable, ShouldHandleClockDisableAndEnable) {
    auto& core = testBench.core();

    core.i_ce = 0;
    testBench.tick(3);

    core.i_ce = 1;
    testBench.tick(3);

    core.i_ce = 0;
    testBench.tick(3);

    core.i_ce = 1;
    testBench.tick(3);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(12)
        .port(o_ce_cpu)
            .signal("______")
            .signal("____--")
            .concat().repeat(2)
        .port(o_ce_ppu)
            .signal("______")
            .signal("------")
            .concat().repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ClockEnable, ShouldHandleLongPeriodOfClockDisable) {
    auto& core = testBench.core();

    core.i_ce = 0;
    testBench.tick(1000);
    testBench.trace.clear();

    core.i_ce = 1;
    testBench.tick(3);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(3)
        .port(o_ce_cpu)
            .signal("____--")
        .port(o_ce_ppu)
            .signal("------");

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}