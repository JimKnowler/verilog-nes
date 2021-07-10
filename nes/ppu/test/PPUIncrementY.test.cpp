#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUIncrementYTestBench.h"
using namespace ppuincrementytestbench;

namespace {
    class PPUIncrementY : public ::testing::TestWithParam<int> {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        PPUIncrementYTestBench testBench;
    };
}

TEST_F(PPUIncrementY, ShouldConstruct) {
    // empty
}
