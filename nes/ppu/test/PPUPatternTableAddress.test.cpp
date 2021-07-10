#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUPatternTableAddressTestBench.h"
using namespace ppupatterntableaddresstestbench;

namespace {
    class PPUPatternTableAddress : public ::testing::TestWithParam<int> {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        PPUPatternTableAddressTestBench testBench;
    };
}

TEST_F(PPUPatternTableAddress, ShouldConstruct) {
    // empty
}
