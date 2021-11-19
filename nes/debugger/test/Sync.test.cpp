#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/SyncTestBench.h"
using namespace synctestbench;

namespace {
    class Sync : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        void tickSync(int numTicks = 1) {
            auto& core = testBench.core();

            for (int i=0; i<numTicks; i++) {
                core.i_sync_clk = 1;
                core.eval();
                testBench.onStep();

                core.i_sync_clk = 0;
                core.eval();
                testBench.onStep();
            }
        }

        SyncTestBench testBench;
    };
}

TEST_F(Sync, ShouldConstruct) {

}

TEST_F(Sync, ShouldNotDetectPosEdgeWhenSyncClkIsNotTicked) {
    auto& core = testBench.core();

    core.i_data = 1;
    testBench.tick(10);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("-_").repeat(10)
        .port(i_sync_clk).signal("__").repeat(10)
        .port(o_sync_posedge).signal("__").repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Sync, ShouldNotGeneratePosEdgeWhenOnlySyncClkIsTicked) {
    tickSync(10);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("__").repeat(10)
        .port(i_sync_clk).signal("-_").repeat(10)
        .port(o_sync_posedge).signal("__").repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Sync, ShouldGeneratePosedge) {
    auto& core = testBench.core();

    core.i_data = 1;
    testBench.tick(1);
    core.i_data = 0;

    tickSync(9);

    Trace expected = TraceBuilder()
        .port(i_clk)
                .signal("-_")
                .signal("__").repeat(9)
        .port(i_sync_clk)
                .signal("__")
                .signal("-_").repeat(9)
        .port(o_sync_posedge)
                .signal("__").repeat(2)
                .signal("--")
                .signal("__").repeat(7);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Sync, ShouldGenerateMultiplePosedges) {
    auto& core = testBench.core();

    for (int i=0; i<2; i++) {
        core.i_data = 1;
        testBench.tick(1);

        tickSync(3);

        core.i_data = 0;
        testBench.tick(1);

        tickSync();
    }   

    Trace expected = TraceBuilder()
        .port(i_clk)
                .signal("-_")
                .signal("__").repeat(3)
                .signal("-_")
                .signal("__")
                .concat().repeat(2)
        .port(i_sync_clk)
                .signal("__")
                .signal("-_").repeat(3)
                .signal("__")
                .signal("-_")
                .concat().repeat(2)
        .port(o_sync_posedge)
                .signal("__").repeat(2)
                .signal("--")
                .signal("__").repeat(3)
                .concat().repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}