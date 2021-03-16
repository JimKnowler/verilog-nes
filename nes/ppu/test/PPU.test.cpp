#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUTestBench.h"
using namespace pputestbench;

namespace {
    class PPU : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);

            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        PPUTestBench testBench;
    };
}

TEST_F(PPU, ShouldConstruct) {

}

TEST_F(PPU, ShouldInitialiseWithDefaultOutputs) {
    auto& core = testBench.core();

    EXPECT_EQ(1, core.o_int_n);
    EXPECT_EQ(1, core.o_v_we_n);
    EXPECT_EQ(1, core.o_v_rd_n);
}
