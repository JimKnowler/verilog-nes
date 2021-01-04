#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/TCUTestBench.h"
using namespace tcutestbench;

namespace {
    class TCU : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);

            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        void tick(int numTicks) {
            auto& core = testBench.core();

            // simulate incrementing tcu at
            // steady rate

            for (int i=0; i<numTicks; i++) {
                core.i_tcu_next = core.o_tcu + 1;
                testBench.step();

                core.i_tcu_next = core.o_tcu + 1;
                testBench.step();
            }
        }

        TCUTestBench testBench;
    };
}

TEST_F(TCU, ShouldConstruct) {

}

TEST_F(TCU, ShouldReset) {
    testBench.reset();
    testBench.trace.clear();

    tick(1);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
        .port(o_tcu).signal({0}).repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(TCU, ShouldIncrement) {
    tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(4)
        .port(o_tcu).signal({0, 1, 2, 3}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(TCU, ShouldIncrementThenReset) {
    // increment
    tick(4);
    testBench.trace.clear();

    // reset
    testBench.reset();
    
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
        .port(o_tcu).signal({0}).repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(TCU, ShouldEmitSyncDuringT1) {
    tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(4)
        .port(o_tcu)
            .signal({0, 1, 2, 3}).repeatEachStep(2)
        .port(o_sync).signal("_-__").repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
