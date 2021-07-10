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

TEST_F(PPUAttributeAddress, ShouldOutputCorrectAddress) {
    auto& core = testBench.core();

    for (uint16_t v=0; v < (1<<15); v++) {
        core.i_v = v;
        core.eval();

        ASSERT_EQ(core.o_address, 0x23C0 | (v & 0x0C00) | ((v >> 4) & 0x38) | ((v >> 2) & 0x07));
    }
}
