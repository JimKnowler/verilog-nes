#include <numeric>

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

const int SCREEN_WIDTH = 341;
const int SCREEN_HEIGHT = 262;
const int SCREEN_VISIBLE_WIDTH = 256;
const int SCREEN_VISIBLE_HEIGHT = 240;

const int PPUCTRL_V = 1<<7;             // V - NMI enable for VBlank
const int PPUSTATUS_V = 1<<7;           // V - Currently in VBlank

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

TEST_F(PPU, ShouldImplementPixelClock) {
    testBench.tick(SCREEN_WIDTH * SCREEN_HEIGHT);

    std::vector<uint32_t> pixelClockX(SCREEN_WIDTH);
    std::iota(std::begin(pixelClockX), std::end(pixelClockX), 0);

    std::vector<uint32_t> pixelClockY(SCREEN_HEIGHT);
    std::iota(std::begin(pixelClockY), std::end(pixelClockY), 0);

    Trace expected = TraceBuilder()
        .port(o_video_visible)
            .signal("1").repeat(SCREEN_VISIBLE_WIDTH)
            .signal("0").repeat(SCREEN_WIDTH - SCREEN_VISIBLE_WIDTH)
                .concat()
                .repeat(SCREEN_VISIBLE_HEIGHT)
            .signal("0").repeat(SCREEN_WIDTH)
                .repeat(SCREEN_HEIGHT - SCREEN_VISIBLE_HEIGHT)
            .concat()
            .repeatEachStep(2)
        .port(o_video_x)
            .signal(pixelClockX).repeat(SCREEN_HEIGHT)
            .repeatEachStep(2)
        .port(o_video_y)
            .signal(pixelClockY).repeatEachStep(SCREEN_WIDTH)
            .repeatEachStep(2);
    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(PPU, ShouldInvokeNonMaskableInterruptDuringVBlank) {
    auto& core = testBench.core();

    core.i_rs = RS_PPUCTRL;
    core.i_rw = RW_WRITE;
    core.i_data = PPUCTRL_V;        // enable NMI
    core.eval();

    testBench.tick(SCREEN_WIDTH * SCREEN_HEIGHT * 2);

    Trace expected = TraceBuilder()
        .port(o_int_n)
            .signal("1").repeat((SCREEN_WIDTH * 242) + 1)
            .signal("0").repeat((SCREEN_WIDTH * 20) - 1)           // NMI set on 2nd tick of 242nd line
            .concat()
            .repeatEachStep(2)
            .repeat(2);

    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(PPU, ShouldNotInvokeNonMaskableInterruptDuringVBlankWhenDisabledOnPPUCTRL) {
    auto& core = testBench.core();

    core.i_rs = RS_PPUCTRL;
    core.i_rw = RW_WRITE;
    core.i_data = 0;        // disable NMI
    core.eval();

    testBench.tick(SCREEN_WIDTH * SCREEN_HEIGHT);

    Trace expected = TraceBuilder()
        .port(o_int_n)
            .signal("1").repeat(SCREEN_WIDTH * SCREEN_HEIGHT)
            .repeatEachStep(2);

    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(PPU, ShouldClearVBlankNonMaskableInterruptWhenReadingPPUSTATUS) {
    auto& core = testBench.core();

    core.i_rs = RS_PPUCTRL;
    core.i_rw = RW_WRITE;
    core.i_data = PPUCTRL_V;        // enable NMI
    core.eval();

    // skip to pixel during vblank
    testBench.tick(SCREEN_WIDTH * 243);
    EXPECT_EQ(0, core.o_int_n);

    // read from PPUSTATUS
    core.i_rs = RS_PPUSTATUS;
    core.i_rw = RW_READ;
    core.eval();
    EXPECT_EQ(PPUSTATUS_V, core.o_data & PPUSTATUS_V);

    // tick the clock, to clear the vblank bit in ppustatus
    testBench.tick();
    EXPECT_EQ(0, core.o_data & PPUSTATUS_V);
}

// - TODO: read ppustatus
//         - DONE: at startup - vblank clear
//         - DONE: when nmi - read vblank as set + clear vblank
//         - continuously - never get vblank
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
