#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/ProcessorStatusTestBench.h"
using namespace processorstatustestbench;

#include "ProcessorStatusFlags.h"

namespace {
    class ProcessorStatus : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
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

    EXPECT_EQ(0, testBench.core().o_p);
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
        .port(o_p).signal({0,N,N}).repeatEachStep(2);

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
        .port(o_p).signal({0}).repeat(6); 

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldLoadZFromZeroDB) {
    auto& core = testBench.core();
    
    core.i_db = 0;
    core.i_dbz_z = 0;
    testBench.tick();

    core.i_dbz_z = 1;
    testBench.tick();

    core.i_dbz_z = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({0,Z,Z}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(ProcessorStatus, ShouldNotLoadZFromNonZeroDB7) {
    auto& core = testBench.core();
    
    core.i_db = 0xff;
    core.i_dbz_z = 0;
    testBench.tick();

    core.i_dbz_z = 1;
    testBench.tick();

    core.i_dbz_z = 0;
    testBench.tick();

    Trace expected = TraceBuilder()
        .port(o_p).signal({0}).repeat(6);

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
        .port(o_p).signal({0,C,C}).repeatEachStep(2);

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
        .port(o_p).signal({0}).repeatEachStep(6);

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
        .port(o_p).signal({0,C,C}).repeatEachStep(2);

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
        .port(o_p).signal({0,C,0,0}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
