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

        void helperReceiveByte(int value) {
            auto& core = testBench.core();

            core.i_rx_dv = 1;
            core.i_rx_byte = value;
            testBench.tick();
        }

        void helperIdleTick() {
            auto& core = testBench.core();

            core.i_rx_dv = 0;
            core.i_rx_byte = 0;
            testBench.tick();
        }

        DebuggerTestBench testBench;
    };

    enum Command : uint8_t {
        NOP = 0,
        ECHO = 1,
        MEM_WRITE = 2
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
    const int kTestValue = 42;

    // starts in NOP state
    helperIdleTick();
    // receive ECHO cmd
    helperReceiveByte(ECHO);
    helperIdleTick();
    // receive value to echo
    helperReceiveByte(kTestValue);
    helperIdleTick();
    // send echo value back + return to NOP state
    helperReceiveByte(0);
    helperIdleTick();

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

TEST_F(Debugger, ShouldImplementMemoryWrite) {
    const uint16_t kTestAddress = 0xCDEF;
    const int kTestNumBytes = 3;
    const uint16_t kTestBytes[kTestNumBytes] = { 0xA1, 0xB2, 0x42 };

    // starts in NOP state
    helperIdleTick();
    // receive MEM_WRITE cmd
    helperReceiveByte(MEM_WRITE);
    helperIdleTick();
    // receive address to write to (high byte)
    helperReceiveByte((kTestAddress >> 8) & 0xff);
    helperIdleTick();
    // receive address to write to (low byte)
    helperReceiveByte(kTestAddress & 0xff);
    helperIdleTick();
    // receive number of bytes to write (high byte)
    helperReceiveByte(0);
    helperIdleTick();
    // receive number of bytes to write (low byte)
    helperReceiveByte(kTestNumBytes);
    helperIdleTick();
    // receive test data
    for (int i=0; i<kTestNumBytes; i++) {
        helperReceiveByte(kTestBytes[i]);
        helperIdleTick();
    }

    Trace expected = TraceBuilder()
        .port(i_clk).signal("-_")
                    .repeat(17)
        .port(o_cmd).signal({NOP})
                    .signal({MEM_WRITE}).repeat(15)
                    .signal({NOP})
                    .concat().repeatEachStep(2)
        .port(o_tx_dv).signal("0").repeat(17)
                      .concat().repeatEachStep(2)
        .port(o_tx_byte).signal({0}).repeat(17)
                        .concat().repeatEachStep(2)
        .port(o_mem_en)
                        .signal("0").repeat(11)
                        .signal("10").repeat(3)
                        .concat().repeatEachStep(2)
        .port(o_mem_rw) 
                        .signal("1").repeat(11)
                        .signal("01").repeat(3)
                        .concat().repeatEachStep(2)   // 0 when writing to memory
        .port(o_mem_address)
                        .signal({0}).repeat(11)
                        .signal({
                            kTestAddress,
                            0,
                            kTestAddress+1,
                            0,
                            kTestAddress+2,
                            0
                        })
                        .concat().repeatEachStep(2)
        .port(o_mem_data)
                        .signal({0}).repeat(11)
                        .signal({
                            kTestBytes[0],
                            0,
                            kTestBytes[1],
                            0,
                            kTestBytes[2],
                            0
                        })
                        .concat().repeatEachStep(2);
                        
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
