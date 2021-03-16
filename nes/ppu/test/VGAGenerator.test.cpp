#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/VGAGenerator3x2TestBench.h"
using namespace vgagenerator3x2testbench;

namespace {
    const uint32_t kWidth = 6;
    const uint32_t kHeight = 3;
    const uint32_t kWidthVisible = 3;
    const uint32_t kHeightVisible = 2;
    
    class VGAGenerator : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(0);

            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        VGAGenerator3x2TestBench testBench;
    };
}

TEST_F(VGAGenerator, ShouldConstruct) {
    auto& core = testBench.core();

    EXPECT_EQ(core.o_visible, 1);
    EXPECT_EQ(core.o_x, 0);
    EXPECT_EQ(core.o_y, 0);
}

TEST_F(VGAGenerator, ShouldReset) {
    auto& core = testBench.core();

    testBench.tick(3000);
    testBench.reset();

    EXPECT_EQ(core.o_x, 0);
    EXPECT_EQ(core.o_y, 0);  
}

std::vector<uint32_t> range(uint32_t end) {
    std::vector<uint32_t> values;

    for (uint32_t i=0; i<end; i++) {
        values.push_back(i);
    }

    return values;
}

TEST_F(VGAGenerator, ShouldGenerateSignal) {
    const uint32_t kNumTicks = kWidth * kHeight;
    testBench.reset();
    testBench.tick(kNumTicks - 1);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("-_")
                    .repeat(kNumTicks)
        .port(o_x).signal(range(kWidth)).repeatEachStep(2).repeat(kHeight)
        .port(o_y).signal(range(kHeight)).repeatEachStep(kWidth).repeatEachStep(2)
        .port(o_visible)
            .signal("-").repeat(kWidthVisible).signal("_").repeat(kWidth - kWidthVisible)
                .concat().repeatEachStep(2).repeat(kHeightVisible)
            .signal("_").repeat(kWidth).repeatEachStep(2).repeat(kHeight - kHeightVisible);
        

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}