#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/ClockDividerTestBench.h"
using namespace clockdividertestbench;

namespace {
    class ClockDivider : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        ClockDividerTestBench testBench;
    };
}

TEST_F(ClockDivider, ShouldConstruct) {
}

TEST_F(ClockDivider, ShouldReset) {
    testBench.reset();

    auto& core = testBench.core();

    ASSERT_EQ(core.o_clk_cpu, 0);
    ASSERT_EQ(core.o_clk_ppu, 0);
    ASSERT_EQ(core.o_cs_n_ppu, 0);
    ASSERT_EQ(core.o_clk_mcu, 0);
}

TEST_F(ClockDivider, ShouldDivideClock) {
    testBench.tick(24);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(12)
        .port(o_clk_cpu).signal("_-").repeatEachStep(12)
        .port(o_clk_ppu).signal("_-").repeatEachStep(4).repeat(3)
        .port(o_clk_mcu).signal("_-").repeatEachStep(2).repeat(6)
        .port(o_cs_n_ppu)
            .signal("_")
            .signal("-").repeat(22)
            .signal("_");

    expected = expected + expected;

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
