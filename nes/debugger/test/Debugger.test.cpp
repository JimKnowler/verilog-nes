#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/DebuggerTestBench.h"
using namespace debuggertestbench;

namespace {
    class Debugger : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(0);

            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        DebuggerTestBench testBench;
    };

    enum Command : uint8_t {
        NOP = 0,
        ECHO = 1
    };
}

TEST_F(Debugger, ShouldConstruct) {

}

TEST_F(Debugger, ShouldReset) {
    testBench.reset();

    auto& core = testBench.core();
    EXPECT_EQ(core.o_cmd, NOP);
    EXPECT_EQ(core.o_cmd_bytes_remaining, 0);
}

TEST_F(Debugger, ShouldImplementCmdNOP) {
    auto& core = testBench.core();

    core.i_rx_dv = 1;
    core.i_rx_byte = NOP;
    testBench.step();

    EXPECT_EQ(core.o_cmd, NOP);
    EXPECT_EQ(core.o_cmd_bytes_remaining, 0);
}

TEST_F(Debugger, ShouldImplementCmdEcho) {
    auto& core = testBench.core();

    const int kTestValue = 42;

    // starts in NOP state
    testBench.tick();

    // receive cmd echo
    core.i_rx_dv = 1;
    core.i_rx_byte = ECHO;
    testBench.tick();

    // gap before next byte received
    core.i_rx_dv = 0;
    core.i_rx_byte = 0;
    testBench.tick();

    // receive byte to echo
    core.i_rx_dv = 1;
    core.i_rx_byte = kTestValue;
    testBench.tick();

    // gap before next byte received
    core.i_rx_dv = 0;
    core.i_rx_byte = 0;
    testBench.tick();

    // send byte back
    core.i_rx_dv = 1;
    core.i_rx_byte = 0;
    testBench.tick();

    // returned to NOP (idle) state
    core.i_rx_dv = 0;
    core.i_rx_byte = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(i_clk).signal("-_")
                    .repeat(7)
        .port(o_cmd).signal({NOP})
                    .signal({ECHO}).repeat(5)
                    .signal({NOP})
                    .concat().repeatEachStep(2)
        .port(o_tx_dv).signal("0").repeat(5)
                      .signal("1")
                      .signal("0")
                      .concat().repeatEachStep(2)
        .port(o_tx_byte).signal({0}).repeat(5)
                        .signal({kTestValue})
                        .signal({0})
                        .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

