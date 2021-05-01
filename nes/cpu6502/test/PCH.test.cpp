#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PCHTestBench.h"
using namespace pchtestbench;

namespace {
    class PCH : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.core().i_ce = 1;
            testBench.reset();
        }
        
        void TearDown() override {
        }

        PCHTestBench testBench;
    };
}

TEST_F(PCH, ShouldConstruct) {
}

TEST_F(PCH, ShouldReset) {
    testBench.reset();
}

TEST_F(PCH, ShouldReadFromADH) {
    auto& core = testBench.core();

    core.i_adh = 0xAB;
    core.i_adh_pch = 1;
    core.eval();
    EXPECT_EQ(0, core.o_pch);

    testBench.tick();
    EXPECT_EQ(0xAB, core.o_pch);
}

TEST_F(PCH, ShouldReadFromADHAndIncrement) {
    auto& core = testBench.core();

    core.i_adh = 0xEA;
    core.i_adh_pch = 1;
    core.i_pclc = 1;
    core.eval();
    EXPECT_EQ(0, core.o_pch);

    testBench.tick();
    EXPECT_EQ(0xEB, core.o_pch);
}

TEST_F(PCH, ShouldReusePCH) {
    auto& core = testBench.core();

    // load value into PCH
    core.i_adh = 0xEA;
    core.i_adh_pch = 1;
    testBench.tick();
    
    // now reuse the existing value
    core.i_adh = 0x00;
    core.i_adh_pch = 0;
    core.i_pch_pch = 1;
    testBench.tick();
    EXPECT_EQ(0xEA, core.o_pch);
}

TEST_F(PCH, ShouldIncrementPCH) {
    auto& core = testBench.core();

    // load value into PCH
    core.i_adh = 0xEA;
    core.i_adh_pch = 1;
    testBench.tick();
    
    // now increment the existing value
    core.i_adh = 0x00;
    core.i_adh_pch = 0;
    core.i_pch_pch = 1;
    core.i_pclc = 1;
    testBench.tick();
    EXPECT_EQ(0xEB, core.o_pch);
}

TEST_F(PCH, ShouldNotPassThroughDuringPhi2) {
    auto& core = testBench.core();

    // phi 2
    core.i_clk = 1;
    core.eval();

    core.i_adh = 0xB3;
    core.i_adh_pch = 1;
    core.eval();

    EXPECT_EQ(0x00, core.o_pch);
}

TEST_F(PCH, ShouldNotPassThroughDuringPhi1) {
    auto& core = testBench.core();

    // phi 1
    core.i_clk = 0;
    core.eval();

    // phi 1
    core.i_adh = 0xB3;
    core.i_adh_pch = 1;
    core.eval();

    EXPECT_EQ(0x00, core.o_pch);
}

TEST_F(PCH, ShouldLatchAtEndOfPhi2) {
    auto& core = testBench.core();

    // phi 2
    core.i_clk = 1;
    core.eval();

    // opportunity to load data
    core.i_adh = 0xB3;
    core.i_adh_pch = 1;
    core.eval();

    // phi 1
    core.i_clk = 0;
    core.eval();

    EXPECT_EQ(0xB3, core.o_pch);
}

TEST_F(PCH, ShouldNotLatchAtEndOfPhi1) {
    auto& core = testBench.core();

    // phi 1
    core.i_clk = 0;
    core.eval();

    // opportunity to load data
    core.i_adh = 0xD2;
    core.i_adh_pch = 1;

    // phi 2
    core.i_clk = 1;
    core.eval();

    EXPECT_EQ(0, core.o_pch);
}

TEST_F(PCH, ShouldNotLoadWhileClockDisabled) {
    auto& core = testBench.core();
    core.i_ce = 0;

    // load value into PCH
    core.i_adh = 0xEA;
    core.i_adh_pch = 1;
    testBench.tick();
    
    EXPECT_EQ(0, core.o_pch);
}