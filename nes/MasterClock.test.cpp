#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/MasterClockTestBench.h"
using namespace masterclocktestbench;

namespace {
    class MasterClock : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        MasterClockTestBench testBench;
    };
}

TEST_F(MasterClock, ShouldConstruct) {
}

TEST_F(MasterClock, ShouldReset) {
    testBench.reset();

    ASSERT_EQ(testBench.core().o_clk_cpu, 0);
    ASSERT_EQ(testBench.core().o_clk_ppu, 0);
    ASSERT_EQ(testBench.core().o_clk_vga, 0);
    ASSERT_EQ(testBench.core().o_clk_mcu, 0);
}