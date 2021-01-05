#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/AddressBusRegisterTestBench.h"
using namespace addressbusregistertestbench;

namespace {
    class AddressBusRegister : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        AddressBusRegisterTestBench testBench;
    };
}

TEST_F(AddressBusRegister, ShouldConstruct) {
    auto& core = testBench.core();

    EXPECT_EQ(0x00, core.o_address);
}

TEST_F(AddressBusRegister, ShouldReset) {
    testBench.reset();
}

TEST_F(AddressBusRegister, ShouldNotPassThroughDuringPhi2) {
    auto& core = testBench.core();

    // phi2
    core.i_clk = 1;
    core.eval();

    core.i_load = 1;
    core.i_address = 0x42;
    core.eval();

    EXPECT_EQ(0x00, core.o_address);
}

TEST_F(AddressBusRegister, ShouldPassThroughDuringPhi1) {
    auto& core = testBench.core();

    // phi1
    core.i_clk = 0;
    core.eval();

    // enable load
    core.i_load = 1;
    core.i_address = 0x42;
    core.eval();

    EXPECT_EQ(0x42, core.o_address);
}

TEST_F(AddressBusRegister, ShouldLatchAddressAtFallingEdgeOfPhi1) {
    auto& core = testBench.core();

    // phi2
    core.i_clk = 1;
    core.eval();

    // enable load
    core.i_load = 1;
    core.i_address = 0x42;
    core.eval();

    // phi 1
    core.i_clk = 0;
    core.eval();

    // should latch on falling edge of phi1
    core.i_clk = 1;
    core.eval();
    EXPECT_EQ(0x42, core.o_address);
}

TEST_F(AddressBusRegister, ShouldNotLatchAddressAtLeadingEdgeOfPhi1) {
    auto& core = testBench.core();

    // phi2
    core.i_clk = 1;
    core.eval();

    // enable load
    core.i_load = 1;
    core.i_address = 0x42;
    core.eval();

    // phi 1 - leading edge
    core.i_clk = 0;
    core.eval();

    // should latch on falling edge of phi1
    core.i_clk = 1;
    core.i_address = 0x5B;
    core.eval();
    EXPECT_EQ(0x5B, core.o_address);
}

TEST_F(AddressBusRegister, ShouldNotPassThroughDuringPhi1WithoutLoadSignal) {
    auto& core = testBench.core();

    // phi1
    core.i_clk = 0;
    core.eval();

    // set input data, but not the load signal
    core.i_load = 0;
    core.i_address = 0x42;
    core.eval();

    // should not pass through input address
    EXPECT_EQ(0x00, core.o_address);
}

TEST_F(AddressBusRegister, ShouldNotLatchAtFallingEdgeOfPhi1WithoutLoadSignal) {
    auto& core = testBench.core();

    // phi2
    core.i_clk = 1;
    core.eval();

    // set input address, but disable load
    core.i_load = 0;
    core.i_address = 0x42;
    core.eval();

    // phi 1
    core.i_clk = 0;
    core.eval();

    // should not latch on falling edge of phi1
    core.i_clk = 1;
    core.eval();
    EXPECT_EQ(0x00, core.o_address);
}
