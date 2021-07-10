#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUIncrementXTestBench.h"
using namespace ppuincrementxtestbench;

namespace {
    class PPUIncrementX : public ::testing::TestWithParam<int> {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        PPUIncrementXTestBench testBench;
    };
}

TEST_F(PPUIncrementX, ShouldConstruct) {
    // empty
}
