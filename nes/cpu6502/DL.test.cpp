#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/DLTestBench.h"
using namespace dltestbench;

namespace {
    class DL : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        DLTestBench testBench;
    };
}

TEST_F(DL, ShouldConstruct) {
    auto& core = testBench.core();

    EXPECT_EQ(0x00, core.o_data);
}

TEST_F(DL, ShouldReset) {
    testBench.reset();
}

TEST_F(DL, ShouldLoadData) {
    auto& core = testBench.core();

    core.i_clk = 1;
    core.eval();

    core.i_data = 0x42;

    // should not load with pure combinatorial
    core.eval();
    EXPECT_EQ(0x00, core.o_data);

    // should not load on falling edge of clk
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(0x00, core.o_data);

    // should load on rising edge of clk
    core.i_clk = 1;
    core.eval();
    EXPECT_EQ(0x42, core.o_data);
}
