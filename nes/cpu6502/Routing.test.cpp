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

TEST_F(Routing, ShouldRoutePchToAdh) {
    auto& core = testBench.core();

    core.i_pch = 0xC5;
    core.i_pch_adh = 1;
    core.eval();

    EXPECT_EQ(0xC5, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);
}

TEST_F(Routing, ShouldRoutePchToDb) {
    auto& core = testBench.core();

    core.i_pch = 0xE2;
    core.i_pch_db = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xE2, core.o_bus_db);
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

TEST_F(Routing, ShouldRouteXtoSb) {
    auto& core = testBench.core();

    core.i_x = 0x4C;
    core.i_x_sb = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0x4C, core.o_bus_sb);    
}

TEST_F(Routing, ShouldRouteYtoSb) {
    auto& core = testBench.core();

    core.i_y = 0x5D;
    core.i_y_sb = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0x5D, core.o_bus_sb);   
}

TEST_F(Routing, ShouldRouteAcToSb) {
    auto& core = testBench.core();

    core.i_ac = 0x1E;
    core.i_ac_sb = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0x1E, core.o_bus_sb);   
}

TEST_F(Routing, ShouldRouteAcToDb) {
    auto& core = testBench.core();

    core.i_ac = 0xB8;
    core.i_ac_db = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xB8, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb);   
}

TEST_F(Routing, ShouldRouteStackPointerToSb) {
    auto& core = testBench.core();

    core.i_s = 0x3D;
    core.i_s_sb = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0xFF, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0x3D, core.o_bus_sb);   
}

TEST_F(Routing, ShouldRouteStackPointerToAdl) {
    auto& core = testBench.core();

    core.i_s = 0x2E;
    core.i_s_adl = 1;
    core.eval();

    EXPECT_EQ(0xFF, core.o_bus_adh);
    EXPECT_EQ(0x2E, core.o_bus_adl);
    EXPECT_EQ(0xFF, core.o_bus_db);
    EXPECT_EQ(0xFF, core.o_bus_sb); 
}

// TODO: ADD - add/sb(7), add/sb(0-6), add/adl, 
// TODO: Status(P) - p/db
// TODO: Pass Mosfets - sb to adh
// TODO: Pass Mosfest - sb to db
