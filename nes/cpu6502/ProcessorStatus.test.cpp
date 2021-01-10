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

TEST_F(ProcessorStatus, ShouldLoadNFromDB7) {
    auto& core = testBench.core();

    core.i_db7_n = 0;
    core.eval();

    // negative value on db
    core.i_db = (1<<7);

    // should not load while control signal is low
    core.eval();
    EXPECT_EQ(0, core.o_p);

    // should not load on rising edge of control signal
    core.i_db7_n = 1;
    core.eval();
    EXPECT_EQ(0, core.o_p);

    // should load on falling edge of control signal
    core.i_db7_n = 0;
    core.eval();
    EXPECT_EQ(N, core.o_p);    
}

TEST_F(ProcessorStatus, ShouldNotLoadNFromDB7WhenNotSet) {
    auto& core = testBench.core();

    core.i_db7_n = 1;
    core.i_db = 0;
    core.eval();

    core.i_db7_n = 0;
    core.eval();

    EXPECT_EQ(0, core.o_p);  
}
