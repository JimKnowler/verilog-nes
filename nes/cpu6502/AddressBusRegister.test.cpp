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

TEST_F(AddressBusRegister, ShouldLoadAddress) {
    auto& core = testBench.core();

    core.i_clk = 0;
    core.eval();

    core.i_load = 1;
    core.i_address = 0x42;

    // should not load with pure combinatorial
    core.eval();
    EXPECT_EQ(0x00, core.o_address);

    // should not load on rising edge of clk
    core.i_clk = 1;
    core.eval();
    EXPECT_EQ(0x00, core.o_address);

    // should load on falling edge of clk
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(0x42, core.o_address);
}

TEST_F(AddressBusRegister, ShouldNotLoadAddressWithoutLoadControlSignal) {
    auto& core = testBench.core();

    core.i_address = 0x42;
    testBench.tick();

    EXPECT_EQ(0x00, core.o_address);
}
