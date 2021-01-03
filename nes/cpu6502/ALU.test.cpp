#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/ALUTestBench.h"
using namespace alutestbench;

namespace {
    class ALU : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
        }
        
        void TearDown() override {
        }

        ALUTestBench testBench;
    };
}

TEST_F(ALU, ShouldConstruct) {
}

TEST_F(ALU, ShouldAddDbToZeroAndRegisterOutputOnPhi2) {
    auto& core = testBench.core();

    core.i_db = 0xAE;
    core.i_db_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;
    testBench.tick(2);
    
    const Trace expected = TraceBuilder()
        .port(i_clk).signal("_-_-")
        .port(o_add)
            .signal({0})
            .signal({0xAE}).repeat(3);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ALU, ShouldInvertDb) {
    auto& core = testBench.core();

    core.i_db = 0xAE;
    core.i_db_n_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;
    core.eval();

    testBench.tick(2);
    
    const Trace expected = TraceBuilder()
        .port(i_clk).signal("_-_-")
        .port(o_add).signal({0}).signal({0x51}).repeat(3);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ALU, ShouldReset) {
    auto& core = testBench.core();

    core.i_db = 0xAE;
    core.i_db_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;
    testBench.tick();

    testBench.reset();
    EXPECT_EQ(0, core.o_add);
}

TEST_F(ALU, ShouldAddAdlToZero) {
    auto& core = testBench.core();

    core.i_adl = 0xBC;
    core.i_adl_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;
    core.eval();

    testBench.tick(2);
    
    const Trace expected = TraceBuilder()
        .port(i_clk).signal("_-_-")
        .port(o_add).signal({0}).signal({0xBC}).repeat(3);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ALU, ShouldAddAdlToSb) {
    auto& core = testBench.core();

    core.i_adl = 0x11;
    core.i_adl_add = 1;
    core.i_sb = 0x22;
    core.i_sb_add = 1;
    core.i_sums = 1;
    core.eval();

    testBench.tick(2);
    
    const Trace expected = TraceBuilder()
        .port(i_clk).signal("_-_-")
        .port(o_add).signal({0}).signal({0x33}).repeat(3);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}