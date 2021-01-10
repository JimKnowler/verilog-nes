#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/ProcessorStatusTestBench.h"
using namespace processorstatustestbench;

namespace {
    class ProcessorStatus : public ::testing::Test {
    public:
        void SetUp() override {
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
}

