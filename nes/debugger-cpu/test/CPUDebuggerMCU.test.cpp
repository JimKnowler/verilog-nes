#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/CPUDebuggerMCUTestBench.h"
using namespace cpudebuggermcutestbench;

namespace {
    class CPUDebuggerMCU : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        CPUDebuggerMCUTestBench testBench;
    };

    const int RW_READ = 1;
    const int RW_WRITE = 0;
}

TEST_F(CPUDebuggerMCU, ShouldConstruct) {
    auto& core = testBench.core();

    EXPECT_EQ(core.i_debugger_en, 0);
}

TEST_F(CPUDebuggerMCU, ShouldImplementDebuggerRead) {
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

TEST_F(CPUDebuggerMCU, ShouldImplementDebuggerWrite) {
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

TEST_F(CPUDebuggerMCU, ShouldImplementCpuRead) {
    auto& core = testBench.core();

    uint16_t kTestAddress = 0x1234;
    uint8_t kTestData = 0x42;

    core.i_cpu_en = 1;
    core.i_cpu_rw = RW_READ;
    core.i_cpu_address = kTestAddress;
    core.i_mem_data = kTestData;
    core.eval();

    EXPECT_EQ(core.o_mem_en, 1);
    EXPECT_EQ(core.o_mem_wea, 0);
    EXPECT_EQ(core.o_mem_address, kTestAddress);
    EXPECT_EQ(core.o_cpu_data, kTestData);
}

TEST_F(CPUDebuggerMCU, ShouldImplementCpuWrite) {
    auto& core = testBench.core();

    uint16_t kTestAddress = 0xABCD;
    uint8_t kTestData = 0xEA;

    core.i_cpu_en = 1;
    core.i_cpu_rw = RW_WRITE;
    core.i_cpu_address = kTestAddress;
    core.i_cpu_data = kTestData;
    core.eval();

    EXPECT_EQ(core.o_mem_en, 1);
    EXPECT_EQ(core.o_mem_wea, 1);
    EXPECT_EQ(core.o_mem_address, kTestAddress);
    EXPECT_EQ(core.o_mem_data, kTestData);
}

TEST_F(CPUDebuggerMCU, ShouldReturnLastDebuggerReadWhileCpuIsReading) {
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
    core.i_cpu_en = 1;
    core.i_cpu_rw = RW_READ;
    core.i_cpu_address = kTestAddressCpu;
    core.i_mem_data = kTestDataCpu;
    core.eval();

    EXPECT_EQ(core.o_mem_en, 1);
    EXPECT_EQ(core.o_mem_wea, 0);
    EXPECT_EQ(core.o_mem_address, kTestAddressCpu);
    EXPECT_EQ(core.o_cpu_data, kTestDataCpu);
    EXPECT_EQ(core.o_debugger_data, kTestDataDebugger);
}

TEST_F(CPUDebuggerMCU, ShouldReturnLastCpuReadWhileDebuggerIsReading) {
    auto& core = testBench.core();

    uint16_t kTestAddressCpu = 0xFEDC;
    uint8_t kTestDataCpu = 0x34;

    core.i_debugger_en = 0;
    core.i_cpu_en = 1;
    core.i_cpu_rw = RW_READ;
    core.i_cpu_address = kTestAddressCpu;
    core.i_mem_data = kTestDataCpu;
    
    testBench.tick();

    uint16_t kTestAddressDebugger = 0x1234;
    uint8_t kTestDataDebugger = 0x42;

    core.i_cpu_en = 0;
    core.i_debugger_en = 1;
    core.i_debugger_rw = RW_READ;
    core.i_debugger_address = kTestAddressDebugger;
    core.i_mem_data = kTestDataDebugger;
    
    core.eval();

    EXPECT_EQ(core.o_mem_en, 1);
    EXPECT_EQ(core.o_mem_wea, 0);
    EXPECT_EQ(core.o_mem_address, kTestAddressDebugger);
    EXPECT_EQ(core.o_debugger_data, kTestDataDebugger);
    EXPECT_EQ(core.o_cpu_data, kTestDataCpu);
}