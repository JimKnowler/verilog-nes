#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/NESDebuggerMCUTestBench.h"
using namespace nesdebuggermcutestbench;

namespace {
    class NESDebuggerMCU : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.reset();
            testBench.trace.clear();
            testBench.setClockPolarity(1);
        }
        
        void TearDown() override {
        }

        NESDebuggerMCUTestBench testBench;
    };

    const int RW_READ = 1;
    const int RW_WRITE = 0;
}

TEST_F(NESDebuggerMCU, ShouldConstruct) {
    auto& core = testBench.core();

    EXPECT_EQ(core.i_debugger_en, 0);
}

TEST_F(NESDebuggerMCU, ShouldImplementDebuggerRead) {
    auto& core = testBench.core();

    uint16_t kTestAddress = 0x1234;
    uint8_t kTestData = 0x42;

    core.i_debugger_en = 1;
    core.i_debugger_rw = RW_READ;
    core.i_debugger_address = kTestAddress;
    core.i_mem_data = kTestData;
    core.eval();

    EXPECT_EQ(core.o_mem_en, 1);
    EXPECT_EQ(core.o_mem_wea, 0);
    EXPECT_EQ(core.o_mem_address, kTestAddress);
    EXPECT_EQ(core.o_debugger_data, kTestData);
}

TEST_F(NESDebuggerMCU, ShouldImplementDebuggerWrite) {
    auto& core = testBench.core();

    uint16_t kTestAddress = 0xABCD;
    uint8_t kTestData = 0xEA;

    core.i_debugger_en = 1;
    core.i_debugger_rw = RW_WRITE;
    core.i_debugger_address = kTestAddress;
    core.i_debugger_data = kTestData;
    core.eval();

    EXPECT_EQ(core.o_mem_en, 1);
    EXPECT_EQ(core.o_mem_wea, 1);
    EXPECT_EQ(core.o_mem_address, kTestAddress);
    EXPECT_EQ(core.o_mem_data, kTestData);
}

TEST_F(NESDebuggerMCU, ShouldImplementCpuRead) {
    auto& core = testBench.core();

    uint16_t kTestAddress = 0x1234;
    uint8_t kTestData = 0x42;

    core.i_nes_en = 1;
    core.i_nes_rw = RW_READ;
    core.i_nes_address = kTestAddress;
    core.i_mem_data = kTestData;
    core.eval();

    EXPECT_EQ(core.o_mem_en, 1);
    EXPECT_EQ(core.o_mem_wea, 0);
    EXPECT_EQ(core.o_mem_address, kTestAddress);
    EXPECT_EQ(core.o_nes_data, kTestData);
}

TEST_F(NESDebuggerMCU, ShouldImplementCpuWrite) {
    auto& core = testBench.core();

    uint16_t kTestAddress = 0xABCD;
    uint8_t kTestData = 0xEA;

    testBench.trace.clear();

    // clock starts high
    // NES memory write is not active yet
    core.i_nes_en = 0;
    core.i_nes_rw = RW_READ;
    core.i_nes_address = 0;
    core.i_nes_data = 0;
    
    // clock falling edge
    testBench.step();

    // clock rising edge
    testBench.step();

    // NES writes to memory while clock is high
    core.i_nes_en = 1;
    core.i_nes_rw = RW_WRITE;
    core.i_nes_address = kTestAddress;
    core.i_nes_data = kTestData;

    // clock falling edge
    testBench.step();

    // NES write completed when clock went low
    core.i_nes_en = 0;
    core.i_nes_rw = RW_READ;
    core.i_nes_address = 0;
    core.i_nes_data = 0;
    
    // clock rising edge
    testBench.step();

    // clock falling edge
    testBench.step();

    // clock rising edge
    testBench.step();

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-_-_-")
        .port(i_nes_en).signal("__-___")
        .port(i_nes_rw).signal("--_---")
        .port(i_nes_address).signal({0, 0, kTestAddress, 0, 0, 0})
        .port(i_nes_data).signal({0, 0, kTestData, 0, 0, 0})
        .port(o_mem_en).signal("__--__")
        .port(o_mem_wea).signal("__--__")
        .port(o_mem_address).signal({0, 0, kTestAddress, kTestAddress, 0, 0})
        .port(o_mem_data).signal({0, 0, kTestData, kTestData, 0, 0});

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(NESDebuggerMCU, ShouldReturnLastDebuggerReadWhileCpuIsReading) {
    auto& core = testBench.core();

    uint16_t kTestAddressDebugger = 0x1234;
    uint8_t kTestDataDebugger = 0x42;

    core.i_debugger_en = 1;
    core.i_debugger_rw = RW_READ;
    core.i_debugger_address = kTestAddressDebugger;
    core.i_mem_data = kTestDataDebugger;
    
    testBench.tick();

    uint16_t kTestAddressCpu = 0xABCD;
    uint8_t kTestDataCpu = 0xEA;

    core.i_debugger_en = 0;
    core.i_nes_en = 1;
    core.i_nes_rw = RW_READ;
    core.i_nes_address = kTestAddressCpu;
    core.i_mem_data = kTestDataCpu;
    core.eval();

    EXPECT_EQ(core.o_mem_en, 1);
    EXPECT_EQ(core.o_mem_wea, 0);
    EXPECT_EQ(core.o_mem_address, kTestAddressCpu);
    EXPECT_EQ(core.o_nes_data, kTestDataCpu);
    EXPECT_EQ(core.o_debugger_data, kTestDataDebugger);
}

TEST_F(NESDebuggerMCU, ShouldReturnLastCpuReadWhileDebuggerIsReading) {
    auto& core = testBench.core();

    uint16_t kTestAddressCpu = 0xFEDC;
    uint8_t kTestDataCpu = 0x34;

    core.i_debugger_en = 0;
    core.i_nes_en = 1;
    core.i_nes_rw = RW_READ;
    core.i_nes_address = kTestAddressCpu;
    core.i_mem_data = kTestDataCpu;
    
    testBench.tick();

    uint16_t kTestAddressDebugger = 0x1234;
    uint8_t kTestDataDebugger = 0x42;

    core.i_nes_en = 0;
    core.i_debugger_en = 1;
    core.i_debugger_rw = RW_READ;
    core.i_debugger_address = kTestAddressDebugger;
    core.i_mem_data = kTestDataDebugger;
    
    core.eval();

    EXPECT_EQ(core.o_mem_en, 1);
    EXPECT_EQ(core.o_mem_wea, 0);
    EXPECT_EQ(core.o_mem_address, kTestAddressDebugger);
    EXPECT_EQ(core.o_debugger_data, kTestDataDebugger);
    EXPECT_EQ(core.o_nes_data, kTestDataCpu);
}