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

    for (int t=0; t<8; t++) {
        core.i_t = t;
        for (int p=0; p<2; p++) {
            core.i_p = p;
            for (int c=0; c<16; c++) {
                core.i_c = c;
                for (int r=0; r<16; r++) {
                    core.i_r = r;
                    for (int h=0; h<2; h++) {
                        core.i_h = h;
                        core.eval();

                        uint16_t expected = t | (p<<3) | (c<<4) | (r<<8) | (h<<12);
                        ASSERT_EQ(core.o_address, expected);
                    }
                }
            }
        }
    }
}