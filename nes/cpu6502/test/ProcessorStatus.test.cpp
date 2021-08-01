#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/ProcessorStatusTestBench.h"
using namespace processorstatustestbench;

#include "nes/cpu6502/ProcessorStatusFlags.hpp"

namespace {
    class ProcessorStatus : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
            testBench.core().i_clk_en = 1;
            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        ProcessorStatusTestBench testBench;
    };
}

TEST_F(ProcessorStatus, ShouldConstruct) {
    
}

TEST_F(ProcessorStatus, ShouldReset) {
    testBench.reset();

    EXPECT_EQ(U, testBench.core().o_p);
}

TEST_F(ProcessorStatus, ShouldLoadNFromNegativeDB7) {
    auto& core = testBench.core();
    
    core.i_db = (1<<7);     // negative value on db
    core.i_db7_n = 0;
    testBench.tick();

    core.i_db7_n = 1;
    testBench.tick();

    core.i_db7_n = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|N,U|N}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldNotLoadNFromPositiveDB7) {
    auto& core = testBench.core();

    core.i_db = (1 << 7) - 1;       // set all bits except 7
    core.i_db7_n = 0;
    testBench.tick();

    core.i_db7_n = 1;
    testBench.tick();

    core.i_db7_n = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U}).repeat(6); 

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldLoadZFromDBZ) {
    auto& core = testBench.core();
    
    core.i_db = 0;
    core.i_dbz_z = 0;
    testBench.tick();

    core.i_dbz_z = 1;
    testBench.tick();

    core.i_dbz_z = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|Z,U|Z}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldClearZFromDBZ) {
    auto& core = testBench.core();
    
    core.i_db = 0xff;
    core.i_dbz_z = 0;
    testBench.tick();

    core.i_dbz_z = 1;
    testBench.tick();

    core.i_dbz_z = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U}).repeat(6);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldSetZFromDB1) {
    auto& core = testBench.core();
    
    core.i_db = Z;
    core.i_db1_z = 0;
    testBench.tick();

    core.i_db1_z = 1;
    testBench.tick();

    core.i_db1_z = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|Z,U|Z}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldClearZFromDB1) {
    auto& core = testBench.core();
    
    core.i_db = Z;
    core.i_db1_z = 0;
    testBench.tick();

    core.i_db1_z = 1;
    testBench.tick();

    core.i_db = 0;
    testBench.tick();

    core.i_db1_z = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|Z,U,U}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldLoadCFromACR) {
    auto& core = testBench.core();
    
    core.i_acr = 1;
    core.i_acr_c = 0;
    testBench.tick();

    core.i_acr_c = 1;
    testBench.tick();

    core.i_acr_c = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|C,U|C}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldNotLoadCfromZeroACR) {
    auto& core = testBench.core();
    
    core.i_acr = 0;
    core.i_acr_c = 0;
    testBench.tick();

    core.i_acr_c = 1;
    testBench.tick();

    core.i_acr_c = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U}).repeatEachStep(6);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldSetCFromIR5) {
    auto& core = testBench.core();
    
    core.i_ir5= 1;
    core.i_ir5_c = 0;
    testBench.tick();

    core.i_ir5_c = 1;
    testBench.tick();

    core.i_ir5_c = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|C,U|C}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}


TEST_F(ProcessorStatus, ShouldClearCFromIR5) {
    auto& core = testBench.core();
    
    core.i_ir5 = 1;
    core.i_ir5_c = 0;
    testBench.tick();

    core.i_ir5_c = 1;
    testBench.tick();

    core.i_ir5 = 0;
    testBench.tick();

    core.i_ir5_c = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|C,U,U}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldSetCFromDB0) {
    auto& core = testBench.core();
    
    core.i_db = C;
    core.i_db0_c = 0;
    testBench.tick();

    core.i_db0_c = 1;
    testBench.tick();

    core.i_db0_c = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|C,U|C}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}


TEST_F(ProcessorStatus, ShouldClearCFromDB0) {
    auto& core = testBench.core();
    
    core.i_db = C;
    core.i_db0_c = 0;
    testBench.tick();

    core.i_db0_c = 1;
    testBench.tick();

    core.i_db = 0;
    testBench.tick();

    core.i_ir5_c = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|C,U,U}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldSetIFromIR5) {
    auto& core = testBench.core();
    
    core.i_ir5 = 1;
    core.i_ir5_i = 0;
    testBench.tick();

    core.i_ir5_i = 1;
    testBench.tick();

    core.i_ir5_i = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|I,U|I}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldClearIFromIR5) {
    auto& core = testBench.core();
    
    core.i_ir5 = 1;
    core.i_ir5_i = 0;
    testBench.tick();

    core.i_ir5_i = 1;
    testBench.tick();

    core.i_ir5 = 0;
    testBench.tick();

    core.i_ir5_i = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|I,U,U}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldSetIFromDB2) {
    auto& core = testBench.core();
    
    core.i_db = I;
    core.i_db2_i = 0;
    testBench.tick();

    core.i_db2_i = 1;
    testBench.tick();

    core.i_db2_i = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|I,U|I}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldClearIFromDB2) {
    auto& core = testBench.core();
    
    core.i_db = I;
    core.i_db2_i = 0;
    testBench.tick();

    core.i_db2_i = 1;
    testBench.tick();

    core.i_db = 0;
    testBench.tick();

    core.i_db2_i = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|I,U,U}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldSetDFromDB3) {
    auto& core = testBench.core();
    
    core.i_db = D;
    core.i_db3_d = 0;
    testBench.tick();

    core.i_db3_d = 1;
    testBench.tick();

    core.i_db3_d = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|D,U|D}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldClearDFromDB3) {
    auto& core = testBench.core();
    
    core.i_db = D;
    core.i_db3_d = 0;
    testBench.tick();

    core.i_db3_d = 1;
    testBench.tick();

    core.i_db = 0;
    testBench.tick();

    core.i_db3_d = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|D,U,U}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}


TEST_F(ProcessorStatus, ShouldSetDFromIR5) {
    auto& core = testBench.core();
    
    core.i_ir5= 1;
    core.i_ir5_d = 0;
    testBench.tick();

    core.i_ir5_d = 1;
    testBench.tick();

    core.i_ir5_d = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|D,U|D}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}


TEST_F(ProcessorStatus, ShouldClearDFromIR5) {
    auto& core = testBench.core();
    
    core.i_ir5 = 1;
    core.i_ir5_d = 0;
    testBench.tick();

    core.i_ir5_d = 1;
    testBench.tick();

    core.i_ir5 = 0;
    testBench.tick();

    core.i_ir5_d = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|D,U,U}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldSetBFromDB4) {
    auto& core = testBench.core();
    
    core.i_db = B;
    core.i_db4_b = 0;
    testBench.tick();

    core.i_db4_b = 1;
    testBench.tick();

    core.i_db4_b = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|B,U|B}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldClearBFromDB4) {
    auto& core = testBench.core();
    
    core.i_db = B;
    core.i_db4_b = 0;
    testBench.tick();

    core.i_db4_b = 1;
    testBench.tick();

    core.i_db = 0;
    testBench.tick();

    core.i_db4_b = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|B,U,U}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldSetVFromDB6) {
    auto& core = testBench.core();
    
    core.i_db = V;
    core.i_db6_v = 0;
    testBench.tick();

    core.i_db6_v = 1;
    testBench.tick();

    core.i_db6_v = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|V,U|V}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldClearVFromDB6) {
    auto& core = testBench.core();
    
    core.i_db = V;
    core.i_db6_v = 0;
    testBench.tick();

    core.i_db6_v = 1;
    testBench.tick();

    core.i_db = 0;
    testBench.tick();

    core.i_db6_v = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|V,U,U}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldSetVFromAVR) {
    auto& core = testBench.core();
    
    core.i_avr = 1;
    core.i_avr_v = 0;
    testBench.tick();

    core.i_avr_v = 1;
    testBench.tick();

    core.i_avr_v = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|V,U|V}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldClearVFromAVR) {
    auto& core = testBench.core();
    
    core.i_avr = 1;
    core.i_avr_v = 0;
    testBench.tick();

    core.i_avr_v = 1;
    testBench.tick();

    core.i_avr = 0;
    testBench.tick();

    core.i_avr_v = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({U,U|V,U,U}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldNotLoadWhenClockDisabled) {
    auto& core = testBench.core();
    core.i_clk_en = 0;

    core.i_db = 0xff;
    core.i_db0_c = 1;
    core.i_db1_z = 1;
    core.i_db2_i = 1;
    core.i_db3_d = 1;
    core.i_db4_b = 1;
    core.i_db6_v = 1;
    core.i_db7_n = 1;

    testBench.tick();

    EXPECT_EQ(U, core.o_p);
}
