#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUAttributeAddressTestBench.h"
using namespace ppuattributeaddresstestbench;

namespace {
    class PPUAttributeAddress : public ::testing::TestWithParam<int> {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        PPUAttributeAddressTestBench testBench;
    };
}

TEST_F(PPUAttributeAddress, ShouldConstruct) {
    // empty
}
