#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/IRTestBench.h"
using namespace irtestbench;

namespace {
    const uint8_t OPCODE_BRK = 0;

    class IR : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);

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

TEST_F(IR, ShouldFetchOpcodeOnT1) {
    auto& core = testBench.core();

    // tick 1
    core.i_tcu = 0;
    testBench.tick();

    // tick 2
    // note: simulate opcode appearing on data bus at 
    //       start of T1
    core.i_data = 0xAE;
    testBench.step();
    // note: simulate tcu changing after clock edge
    core.i_tcu = 1;
    // note: simulate data bus changing after clock edge
    core.i_data = 0xFF;
    testBench.step();

    // tick 3
    core.i_tcu = 2;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(3)
        .port(o_ir)
            .signal({0}).repeat(3)
            .signal({0xAE}).repeat(3);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(IR, ShouldResetAfterLoadingIR) {
    auto& core = testBench.core();

    // tick 1
    core.i_tcu = 0;
    core.i_data = 0x5C;
    testBench.tick();

    // tick 2
    core.i_tcu = 1;
    testBench.tick();
    
    // tick 3
    core.i_tcu = 2;
    testBench.tick();
    
    testBench.reset();

    EXPECT_EQ(OPCODE_BRK, testBench.core().o_ir);
}
