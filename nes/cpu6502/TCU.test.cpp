#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/TCUTestBench.h"
using namespace tcutestbench;

namespace {
    // maximum value that TCU can output with 4 bits
    const int kMinusOne = 0b1111;

    class TCU : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);

            testBench.setCallbackSimulateCombinatorial([this]{
                auto& core = testBench.core();

                core.i_tcu_next = (core.o_tcu + 1) % (kMinusOne+1);
            });

            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        TCUTestBench testBench;
    };
}

TEST_F(TCU, ShouldConstruct) {

}

TEST_F(TCU, ShouldReset) {
    testBench.reset();
    testBench.trace.clear();

    testBench.tick();
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
        .port(o_tcu).signal({0}).repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(TCU, ShouldIncrement) {
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(4)
        .port(o_tcu).signal({0, 1, 2, 3}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(TCU, ShouldIncrementThenReset) {
    // increment
    testBench.tick(4);
    testBench.trace.clear();

    // reset
    testBench.reset();
    testBench.tick();
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(2)
        .port(o_tcu).signal({kMinusOne, 0}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(TCU, ShouldEmitSyncDuringT1) {
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(4)
        .port(o_tcu)
            .signal({0, 1, 2, 3}).repeatEachStep(2)
        .port(o_sync).signal("_-__").repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
