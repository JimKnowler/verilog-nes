#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PCLTestBench.h"
using namespace pcltestbench;

namespace {
    class PCL : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        PCLTestBench testBench;
    };
}

TEST_F(PCL, ShouldConstruct) {
    EXPECT_EQ(0, testBench.core().o_pcl);
}

TEST_F(PCL, ShouldReset) {
    testBench.reset();
}

TEST_F(PCL, ShouldReadFromADL) {
    auto& core = testBench.core();

    core.i_adl = 0xAB;
    core.i_adl_pcl = 1;
    core.eval();
    EXPECT_EQ(0, core.o_pcl);

    testBench.tick();
    EXPECT_EQ(0xAB, core.o_pcl);
}

TEST_F(PCL, ShouldReadFromADLAndIncrement) {
    auto& core = testBench.core();

    core.i_adl = 0xEA;
    core.i_adl_pcl = 1;
    core.i_i_pc = 1;
    core.eval();
    EXPECT_EQ(0, core.o_pcl);

    testBench.tick();
    EXPECT_EQ(0xEB, core.o_pcl);
}

TEST_F(PCL, ShouldReusePCL) {
    auto& core = testBench.core();

    // load value into PCL
    core.i_adl = 0xEA;
    core.i_adl_pcl = 1;
    testBench.tick();
    
    // now reuse the existing value
    core.i_adl = 0x00;
    core.i_adl_pcl = 0;
    core.i_pcl_pcl = 1;
    testBench.tick();
    EXPECT_EQ(0xEA, core.o_pcl);
}

TEST_F(PCL, ShouldIncrementPCL) {
    auto& core = testBench.core();

    // load value into PCL
    core.i_adl = 0xEA;
    core.i_adl_pcl = 1;
    testBench.tick();
    
    // now increment the existing value
    core.i_adl = 0x00;
    core.i_adl_pcl = 0;
    core.i_pcl_pcl = 1;
    core.i_i_pc = 1;
    testBench.tick();
    EXPECT_EQ(0xEB, core.o_pcl);
}

TEST_F(PCL, ShouldOutputCarry) {
    auto& core = testBench.core();

    // carry should be emitted by combinatorial logic
    core.i_adl = 0xFF;
    core.i_adl_pcl = 1;
    core.eval();
    EXPECT_EQ(0, core.o_pclc);
    EXPECT_EQ(0, core.o_pcl);

    // clock tick should only update o_pcl
    testBench.tick();
    EXPECT_EQ(0, core.o_pclc);
    EXPECT_EQ(0xFF, core.o_pcl);
    
    core.i_i_pc = 1;
    core.eval();
    EXPECT_EQ(1, core.o_pclc);
    EXPECT_EQ(0xFF, core.o_pcl);

    testBench.tick();
    EXPECT_EQ(1, core.o_pclc);
    EXPECT_EQ(0, core.o_pcl);
}

TEST_F(PCL, ShouldNotIncrementWhileResetting) {
    auto& core = testBench.core();

    core.i_i_pc = 1;
    core.i_reset_n = 0;
    testBench.tick(2);

    EXPECT_EQ(0, core.o_pcl);
}


/// TODO: should only increment on phi2 (posedge i_clk) so ready for phi1
///       add a nice signal diagram test to prove it :)