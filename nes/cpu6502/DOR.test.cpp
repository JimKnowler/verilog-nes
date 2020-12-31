#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/DORTestBench.h"
using namespace dortestbench;

namespace {
    class DOR : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.reset();
        }
        
        void TearDown() override {
        }

        DORTestBench testBench;
    };

    const int RW_READ = 1;
    const int RW_WRITE = 0;
}

TEST_F(DOR, ShouldConstruct) {    
    auto& core = testBench.core();

    EXPECT_EQ(0, core.o_data);
}

TEST_F(DOR, ShouldEnableOutput) {
    auto& core = testBench.core();

    // clock in data with tristate disabled
    core.i_rw = RW_READ;
    core.i_data = 0x56;
    testBench.tick();

    // should not enable on falling edge
    core.i_rw = RW_WRITE;
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(0, core.o_data);

    // should enable on rising edge
    core.i_clk = 1;
    core.eval();
    EXPECT_EQ(0x56, core.o_data);
}

TEST_F(DOR, ShouldLoadData) {
    auto& core = testBench.core();

    core.i_data = 0x4E;
    core.i_rw = RW_WRITE;

    // should not register on rising edge
    core.i_clk = 1;
    core.eval();
    EXPECT_EQ(0, core.o_data);
    
    // should register on falling edge
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(0x4E, core.o_data);
}

TEST_F(DOR, ShouldDisableOutputWhenReset) {
    auto& core = testBench.core();

    // clock in i_rw and i_data
    core.i_clk = 0;
    core.i_rw = RW_WRITE;
    core.i_data = 0xFE;
    core.eval();
    core.i_clk = 1;
    core.eval();

    core.i_reset_n = 0;
    core.eval();
    core.i_reset_n = 1;

    // clock in i_data on falling edge
    core.i_clk = 0;
    core.i_data = 0xEE;
    core.eval();
    EXPECT_EQ(0, core.o_data);
}

TEST_F(DOR, ShouldClearDataWhenReset) {
    auto& core = testBench.core();

    // clock in data
    core.i_data = 0xFE;
    core.i_clk = 1;
    core.eval();
    core.i_clk = 0;
    core.eval();

    core.i_reset_n = 0;
    core.eval();
    core.i_reset_n = 1;

    core.i_clk = 1;
    core.i_rw = 0;
    core.eval();
    EXPECT_EQ(0, core.o_data);
}

