#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/CPUDebuggerTestBench.h"
using namespace cpudebuggertestbench;

namespace {
    class CPUDebugger : public ::testing::Test {
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

        void helperIdleTick(int numTicks = 1) {
            auto& core = testBench.core();

            core.i_rx_dv = 0;
            core.i_rx_byte = 0;
            testBench.tick(numTicks);
        }

        CPUDebuggerTestBench testBench;
    };

    enum Command : uint8_t {
        NOP = 0,
        ECHO = 1,
        MEM_WRITE = 2,
        MEM_READ = 3,
        VALUE_WRITE = 4,
        VALUE_READ = 5
    };

    uint8_t hi(uint16_t value) {
        return (value >> 8) & 0xff;
    }

    uint8_t lo(uint16_t value) {
        return value & 0xff;
    }
}

TEST_F(CPUDebugger, ShouldConstruct) {

}

TEST_F(CPUDebugger, ShouldReset) {
    testBench.reset();

    auto& core = testBench.core();
    EXPECT_EQ(core.o_debug_cmd, NOP);
    EXPECT_EQ(core.o_debug_cmd_bytes_remaining, 0);
}

TEST_F(CPUDebugger, ShouldImplementCmdNOP) {
    auto& core = testBench.core();

    core.i_rx_dv = 1;
    core.i_rx_byte = NOP;
    testBench.step();

    EXPECT_EQ(core.o_debug_cmd, NOP);
    EXPECT_EQ(core.o_debug_cmd_bytes_remaining, 0);
}

TEST_F(CPUDebugger, ShouldImplementCmdEcho) {
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
        .port(o_debug_cmd).signal({NOP})
                    .signal({ECHO}).repeat(5)
                    .signal({NOP})
                    .concat().repeatEachStep(2)
        .port(o_tx_dv).signal("0").repeat(4)
                      .signal("1")
                      .signal("0").repeat(2)
                      .concat().repeatEachStep(2)
        .port(o_tx_byte).signal({0}).repeat(4)
                        .signal({kTestValue})
                        .signal({0}).repeat(2)
                        .concat().repeatEachStep(2)
        .port(o_mem_en)
                        .signal("0").repeat(7).repeatEachStep(2)
        .port(o_value_en)
                        .signal("0").repeat(7).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(CPUDebugger, ShouldImplementMemoryWrite) {
    const uint16_t kTestAddress = 0xCDEF;
    const int kTestNumBytes = 3;
    const uint16_t kTestBytes[kTestNumBytes] = { 0xA1, 0xB2, 0x42 };

    // starts in NOP state
    helperIdleTick();
    // receive MEM_WRITE cmd
    helperReceiveByte(MEM_WRITE);
    helperIdleTick();
    // receive address to write to (high byte)
    helperReceiveByte(hi(kTestAddress));
    helperIdleTick();
    // receive address to write to (low byte)
    helperReceiveByte(lo(kTestAddress));
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
        .port(o_debug_cmd).signal({NOP})
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
                        .concat().repeatEachStep(2)
        .port(o_value_en)
                        .signal("0").repeat(17).repeatEachStep(2);
                        
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(CPUDebugger, ShouldImplementMemoryRead) {
    const uint16_t kTestAddress = 0xABCD;
    const int kTestNumBytes = 4;
    const uint16_t kTestBytes[kTestNumBytes] = { 0xD4, 0xC3, 0xB2, 0xA1 };

    auto& core = testBench.core();

    testBench.setCallbackSimulateCombinatorial([kTestBytes, &core]{
        if ((core.o_mem_en == 0) || (core.i_clk == 1)) {
            return;
        }

        int index = core.o_mem_address - kTestAddress;
        if ((index >= 0) && (index < kTestNumBytes)) {
            core.i_mem_data = kTestBytes[index];
        }
    });

    // starts in NOP state
    helperIdleTick();
    // receive MEM_WRITE cmd
    helperReceiveByte(MEM_READ);
    helperIdleTick();
    // receive address to write to (high byte)
    helperReceiveByte(hi(kTestAddress));
    helperIdleTick();
    // receive address to write to (low byte)
    helperReceiveByte(lo(kTestAddress));
    helperIdleTick();
    // receive number of bytes to write (high byte)
    helperReceiveByte(0);
    helperIdleTick();
    // receive number of bytes to read (low byte)
    helperReceiveByte(kTestNumBytes);
    helperIdleTick(2);
    // read test data from memory + transmit it
    for (int i=0; i<kTestNumBytes; i++) {
        helperReceiveByte(0);
        helperIdleTick(2);
    }

    Trace expected = TraceBuilder()
        .port(i_clk).signal("-_")
                    .repeat(24)
        .port(o_debug_cmd).signal({NOP})
                    .signal({MEM_READ}).repeat(22)
                    .signal({NOP})
                    .concat().repeatEachStep(2)
        .port(i_rx_dv)
                    .signal("0")
                    .signal("10").repeat(4)
                    .signal("100").repeat(5)
                    .concat().repeatEachStep(2)
        .port(o_tx_dv).signal("0").repeat(9)
                      .signal("001").repeat(4)
                      .signal("0").repeat(3)
                      .concat().repeatEachStep(2)
        .port(o_tx_byte).signal({0}).repeat(9)
                        .signal({
                            0,
                            0,
                            kTestBytes[0],
                            0,
                            0,
                            kTestBytes[1],
                            0,
                            0,
                            kTestBytes[2],
                            0,
                            0,
                            kTestBytes[3],
                        })
                        .signal({0}).repeat(3)
                        .concat().repeatEachStep(2)
        .port(o_mem_rw) 
                        .signal("1").repeat(24)     // always READ
                        .concat().repeatEachStep(2)
        .port(o_mem_en)
                        .signal("0").repeat(9)
                        .signal("100").repeat(4)
                        .signal("0").repeat(3)
                        .concat().repeatEachStep(2)
        .port(o_mem_address)
                        .signal({0}).repeat(9)
                        .signal({
                            kTestAddress,
                            0,
                            0,
                            kTestAddress+1,
                            0,
                            0,
                            kTestAddress+2,
                            0,
                            0,
                            kTestAddress+3,
                            0,
                            0
                        })
                        .signal({0}).repeat(3)
                        .concat().repeatEachStep(2)
        .port(o_mem_data)
                        .signal({0}).repeat(24)
                        .concat().repeatEachStep(2)
        .port(o_value_en)
                        .signal("0").repeat(24).repeatEachStep(2);
                        
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(CPUDebugger, ShouldSupportGapsBetweenRxBytes) {
    const int kTestValue = 42;

    // starts in NOP state
    helperIdleTick();
    // receive ECHO cmd
    helperReceiveByte(ECHO);
    helperIdleTick(2);
    // receive value to echo
    helperReceiveByte(kTestValue);
    helperIdleTick(3);
    // send echo value back + return to NOP state
    helperReceiveByte(0);
    helperIdleTick();

    Trace expected = TraceBuilder()
        .port(i_clk).signal("-_")
                    .repeat(10)
        .port(o_debug_cmd).signal({NOP})
                    .signal({ECHO}).repeat(8)
                    .signal({NOP})
                    .concat().repeatEachStep(2)
        .port(o_tx_dv).signal("0").repeat(5)
                      .signal("1")
                      .signal("0").repeat(4)
                      .concat().repeatEachStep(2)
        .port(o_tx_byte).signal({0}).repeat(5)
                        .signal({kTestValue})
                        .signal({0}).repeat(4)
                        .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(CPUDebugger, ShouldImplementValueWrite) {
    const uint16_t kTestValueID = 0x1234;
    const uint16_t kTestValue = 0xABCD;

    // starts in NOP state
    helperIdleTick();
    // receive cmd
    helperReceiveByte(VALUE_WRITE);
    helperIdleTick();
    // receive valueId to write to (high byte)
    helperReceiveByte(hi(kTestValueID));
    helperIdleTick();
    // receive valudId to write to (low byte)
    helperReceiveByte(lo(kTestValueID));
    helperIdleTick();
    // receive value to write (high byte)
    helperReceiveByte(hi(kTestValue));
    helperIdleTick();
    // receive value to write (low byte)
    helperReceiveByte(lo(kTestValue));
    helperIdleTick();
    
    Trace expected = TraceBuilder()
        .port(i_clk).signal("-_")
                    .repeat(11)
        .port(o_debug_cmd)
                    .signal({NOP})
                    .signal({VALUE_WRITE}).repeat(9)
                    .signal({NOP})
                    .concat().repeatEachStep(2)
        .port(o_tx_dv).signal("0").repeat(11)
                      .concat().repeatEachStep(2)
        .port(o_tx_byte).signal({0}).repeat(11)
                        .concat().repeatEachStep(2)
        .port(o_value_en)
                        .signal("0").repeat(9)
                        .signal("10")
                        .concat().repeatEachStep(2)
        .port(o_value_rw) 
                        .signal("1").repeat(9)
                        .signal("01")
                        .concat().repeatEachStep(2)   // 0 when writing to memory
        .port(o_value_id)
                        .signal({0}).repeat(9)
                        .signal({
                            kTestValueID,
                            0
                        })
                        .concat().repeatEachStep(2)
        .port(o_value_data)
                        .signal({0}).repeat(9)
                        .signal({
                            kTestValue,
                            0
                        })
                        .concat().repeatEachStep(2)
        .port(o_mem_en)
                        .signal("0").repeat(11).repeatEachStep(2);
                        
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(CPUDebugger, ShouldImplementValueRead) {
    const uint16_t kTestValueID = 0xABCD;
    const uint16_t kTestValue = 0x1234;

    auto& core = testBench.core();

    testBench.setCallbackSimulateCombinatorial([&core]{
        if (core.o_value_en == 0) {
            core.i_value_data = 0;
            return;
        }

        core.i_value_data = kTestValue;
    });

    // starts in NOP state
    helperIdleTick();

    // receive cmd
    helperReceiveByte(VALUE_READ);
    helperIdleTick();
    // receive value ID to write to (high byte)
    helperReceiveByte(hi(kTestValueID));
    helperIdleTick();
    // receive value ID to write to (low byte)
    helperReceiveByte(lo(kTestValueID));
    helperIdleTick();
    // transmit value (high byte)
    helperReceiveByte(0);
    helperIdleTick();
    // transmit value (low byte)
    helperReceiveByte(0);
    helperIdleTick();

    Trace expected = TraceBuilder()
        .port(i_clk).signal("-_")
                    .repeat(11)
        .port(o_debug_cmd).signal({NOP})
                    .signal({VALUE_READ}).repeat(9)
                    .signal({NOP})
                    .concat().repeatEachStep(2)
        .port(i_rx_dv)
                    .signal("0")
                    .signal("10").repeat(5)
                    .concat().repeatEachStep(2)
        .port(o_tx_dv).signal("0").repeat(6)
                      .signal("10").repeat(2)
                      .signal("0")
                      .concat().repeatEachStep(2)
        .port(o_tx_byte).signal({0}).repeat(6)
                        .signal({
                            hi(kTestValue),
                            0,
                            lo(kTestValue),
                            0,
                            0
                        })
                        .concat().repeatEachStep(2)
        .port(o_value_rw) 
                        .signal("1").repeat(11)
                        .concat().repeatEachStep(2)
        .port(o_value_en)
                        .signal("0").repeat(5)
                        .signal("1")
                        .signal("0").repeat(5)
                        .concat().repeatEachStep(2)
        .port(o_value_id)
                        .signal({0}).repeat(5)
                        .signal({kTestValueID})
                        .signal({0}).repeat(5)
                        .concat().repeatEachStep(2)
        .port(o_value_data)
                        .signal({0}).repeat(11)
                        .concat().repeatEachStep(2)
        .port(o_mem_en)
                        .signal("0").repeat(11).repeatEachStep(2);
                        
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}