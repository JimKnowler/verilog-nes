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

TEST_F(PPUPatternTableAddress, ShouldOutputCorrectAddress) {
    auto& core = testBench.core();

    for (uint16_t v=0; v < (1<<15); v++) {
        core.i_v = v;
        core.eval();

        ASSERT_EQ(core.o_address, 0x2000 | (v & 0x0FFF));
    }
}