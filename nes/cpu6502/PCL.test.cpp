#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PCLTestBench.h"
using namespace pcltestbench;

namespace {
    class PCL : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        PCLTestBench testBench;
    };
}

TEST_F(PCL, ShouldConstruct) {
}

TEST_F(PCL, ShouldReset) {
    testBench.reset();
}