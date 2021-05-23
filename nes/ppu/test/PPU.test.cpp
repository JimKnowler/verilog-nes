#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUTestBench.h"
using namespace pputestbench;

const int RS_PPUCTRL   = 0;
const int RS_PPUMASK   = 1;
const int RS_PPUSTATUS = 2;
const int RS_OAMADDR   = 3;
const int RS_OAMDATA   = 4;
const int RS_PPUSCROLL = 5;
const int RS_PPUADDR   = 6;
const int RS_PPUDATA   = 7;

const int RW_READ = 1;
const int RW_WRITE = 0;

namespace {
    class PPU : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);

            testBench.reset();
            testBench.trace.clear();

            // enable chip select
            testBench.core().i_cs_n = 0;
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
    EXPECT_EQ(1, core.o_video_we_n);
    EXPECT_EQ(1, core.o_video_rd_n);

    core.i_rs = RS_PPUSTATUS;
    core.i_rw = RW_READ;
    core.eval();
    EXPECT_EQ(0, core.o_data);
}

TEST_F(PPU, ShouldWriteToPPUCTRL) {
    auto& core = testBench.core();

    core.i_rs = RS_PPUCTRL;
    core.i_rw = RW_WRITE;
    core.i_data = 0xFF;
    core.eval();
    EXPECT_EQ(0, core.o_debug_ppuctrl);
    
    testBench.tick(1);
    EXPECT_EQ(0xFF, core.o_debug_ppuctrl);

    // should not affect other registers
    EXPECT_EQ(0, core.o_debug_ppumask);
}

TEST_F(PPU, ShouldWriteToPPUMASK) {
    auto& core = testBench.core();

    core.i_rs = RS_PPUMASK;
    core.i_rw = RW_WRITE;
    core.i_data = 0xFF;
    core.eval();
    EXPECT_EQ(0, core.o_debug_ppumask);
    
    testBench.tick(1);
    EXPECT_EQ(0xFF, core.o_debug_ppumask);

    // should not affect other registers
    EXPECT_EQ(0, core.o_debug_ppuctrl);
}

// - TODO: o_nmi_n for vblank start/stop + repeats
//         - when ppuctrl.v == 1 (not when 0)
// - TODO: read ppustatus
//         - at startup - vblank clear
//         - when nmi - vlank set + clear vblank
// - TODO: write oamaddr - verify by reading from o_debug_oamaddr
// - TODO: write oamdata - verify o_debug_oamaddr
//          - auto increment oamaddr
// - TODO: read oamdata
//          - does NOT auto increment oamaddr
// - TODO: write ppuscroll - verify by reading from o_debug_scroll_x, 
//                           o_debug_scroll_y, w
//         - 2 x writes (x, y)
// - TODO: reset w - write ppuscroll x 1, read ppustatus
// - TODO: write ppuaddr - verify by reading from o_debug_ppuaddr, w
//         - 2 x writes (mxb, lsb)
// - TODO: write ppudata
//          - autoincrement ppuaddr
// - TODO: read ppudata
//          - autoincrement ppuaddr

