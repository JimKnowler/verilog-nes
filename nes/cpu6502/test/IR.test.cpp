#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/IRTestBench.h"
using namespace irtestbench;

namespace {
    const uint8_t OPCODE_BRK = 0x00;
    const uint8_t OPCODE_NOP = 0xAE;

    class IR : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
            testBench.core().i_clk_en = 1;
            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        IRTestBench testBench;
    };
}

TEST_F(IR, ShouldConstruct) {

}

TEST_F(IR, ShouldReset) {
    testBench.reset();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);
}

TEST_F(IR, ShouldNotLoadOpcodeAtT1AfterReset) {
    testBench.reset();

    auto& core = testBench.core();

    // start phi2
    core.i_tcu_next = 1;
    core.i_data = OPCODE_NOP;
    core.eval();

    // end phi2
    core.i_clk = 0;
    core.i_tcu_next = 2;
    core.eval();

    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);
}

TEST_F(IR, ShouldLoadOpcodesAfterHandlingInterrupt) {
    testBench.reset();

    auto& core = testBench.core();
    core.i_data = OPCODE_NOP;

    // start phi2 (T0 - BRK)
    core.i_tcu_next = 1;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);

    // end phi2 (T0 - BRK)
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);

    // start phi2 (T1 - BRK)
    core.i_clk = 1;
    core.i_tcu_next = 0;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);

    // end phi2 (T1 - BRK)
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);

    // start phi2 (T0 - NOP)
    core.i_clk = 1;
    core.i_tcu_next = 1;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);

    // end phi2 (T0 - NOP)
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(OPCODE_NOP, testBench.core().o_ir);
}

TEST_F(IR, ShouldNotLoadWhenClockDisabled) {
    testBench.reset();

    auto& core = testBench.core();
    core.i_data = OPCODE_NOP;
    core.i_clk_en = 0;

    // start phi2 (T0 - BRK)
    core.i_tcu_next = 1;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);

    // end phi2 (T0 - BRK)
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);

    // start phi2 (T1 - BRK)
    core.i_clk = 1;
    core.i_tcu_next = 0;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);

    // end phi2 (T1 - BRK)
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);

    // start phi2 (T0 - NOP)
    core.i_clk = 1;
    core.i_tcu_next = 1;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);

    // end phi2 (T0 - NOP) - where IR would normally load the instruction if clock enabled
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);
}
