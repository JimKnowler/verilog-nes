#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/RoutingTestBench.h"
using namespace routingtestbench;

namespace {
    class Routing : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        RoutingTestBench testBench;
    };
}

TEST_F(Routing, ShouldConstruct) {    
}

TEST_F(Routing, ShouldPrechargeMosfets) {
    auto& core = testBench.core();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldRouteDlToDb) {
    auto& core = testBench.core();

    core.i_dl = 0xAE;
    core.i_dl_db = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xAE, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldRouteDlToAdl) {
    auto& core = testBench.core();

    core.i_dl = 0xEA;
    core.i_dl_adl = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xEA, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldRouteDlToAdh) {
    auto& core = testBench.core();

    core.i_dl = 0xBE;
    core.i_dl_adh = 1;
    core.eval();

    EXPECT_EQ(0xBE, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldRoutePclToAdl) {
    auto& core = testBench.core();

    core.i_pcl = 0xA4;
    core.i_pcl_adl = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xA4, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldRoutePclToDB) {
    auto& core = testBench.core();

    core.i_pcl = 0xB4;
    core.i_pcl_db = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xB4, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldPullDownAdl0) {
    auto& core = testBench.core();

    core.i_0_adl0 = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFE, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldPullDownAdl1) {
    auto& core = testBench.core();

    core.i_0_adl1 = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFD, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldPullDownAdl2) {
    auto& core = testBench.core();

    core.i_0_adl2 = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFB, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldPullDownAdh0) {
    auto& core = testBench.core();

    core.i_0_adh0 = 1;
    core.eval();

    EXPECT_EQ(0xFE, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldPullDownAdh1_7) {
    auto& core = testBench.core();

    core.i_0_adh1_7 = 1;
    core.eval();

    EXPECT_EQ(0x01, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}


// TODO: PCH
// TODO: Accumulator - ac/db, ac/sb
// TODO: Stack Pointer s/sb, s/adl, 
// TODO: Pass Mosfets - sb to adh
// TODO: Pass Mosfest - sb to db