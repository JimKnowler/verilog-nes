#include <numeric>

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUTestBench.h"
using namespace pputestbench;

#include "nes/memory/SRAM.hpp"

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

const int PPUCTRL_I = 1<<2;             // I - horiz/vertical increment to PPUADDR
const int PPUCTRL_V = 1<<7;             // V - NMI enable for VBlank

const int PPUSTATUS_V = 1<<7;           // V - Currently in VBlank

namespace {
    class PPU : public ::testing::Test {
    public:
        PPU(): vram(0x4000) {}

        void SetUp() override {
            auto& core = testBench.core();

            testBench.setClockPolarity(1);

            testBench.reset();
            testBench.trace.clear();

            // enable chip select
            core.i_cs_n = 0;

            vram.clear(0);

            // simulate vram access
            testBench.setCallbackSimulateCombinatorial([this, &core]{
                if (core.i_clk == 1) {
                    // clock: end of phi2
                    // R/W data is valid on the bus
                    if (core.o_video_we_n == 0) {
                        // write
                        vram.write(core.o_video_address, core.o_video_data);
                    } else if (core.o_video_rd_n == 0) {
                        // read
                        core.i_video_data = vram.read(core.o_video_address);
                    }
                } else {
                    // clock: end of phi 1
                    // undefined data on the bus
                    core.i_video_data = 0xFF;
                }
            });
        }
        
        void TearDown() override {
        }

        void helperDisableRendering() {
            auto& core = testBench.core();

            core.i_rs = RS_PPUMASK;
            core.i_rw = RW_WRITE;
            core.i_data = 0x00;
            testBench.tick();
        }

        void helperSetVerticalVRAMIncrement() {
            auto& core = testBench.core();

            core.i_rs = RS_PPUCTRL;
            core.i_rw = RW_WRITE;
            core.i_data = PPUCTRL_I;
            testBench.tick();
        }

        PPUTestBench testBench;
        memory::SRAM vram;
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
    
    testBench.tick();
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
    
    testBench.tick();
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

TEST_F(PPU, ShouldNotInvokeNonMaskableInterruptDuringVBlankWhileReadingPPUSTATUS) {
    auto& core = testBench.core();

    core.i_rs = RS_PPUCTRL;
    core.i_rw = RW_WRITE;
    core.i_data = PPUCTRL_V;        // enable NMI
    testBench.tick();
    testBench.trace.clear();

    core.i_rs = RS_PPUSTATUS;
    core.i_rw = RW_READ;

    testBench.tick(SCREEN_WIDTH * SCREEN_HEIGHT);

    Trace expected = TraceBuilder()
        .port(o_int_n)
            .signal("1").repeat(SCREEN_WIDTH * SCREEN_HEIGHT)
            .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(PPU, ShouldWritePPUSCROLL) {
    auto& core = testBench.core();

    const uint kTestX = 42;
    const uint kTestY = 87;

    core.i_rs = RS_PPUSCROLL;
    core.i_rw = RW_WRITE;
    
    // should only write X on clock tick
    core.i_data = kTestX;
    core.eval();
    EXPECT_EQ(0, core.o_debug_ppuscroll_x);
    EXPECT_EQ(0, core.o_debug_ppuscroll_y);
    EXPECT_EQ(0, core.o_debug_w);
    
    testBench.tick();
    EXPECT_EQ(kTestX, core.o_debug_ppuscroll_x);
    EXPECT_EQ(0, core.o_debug_ppuscroll_y);
    EXPECT_EQ(1, core.o_debug_w);

    // should only write Y on clock tick
    core.i_data = kTestY;
    core.eval();
    EXPECT_EQ(kTestX, core.o_debug_ppuscroll_x);
    EXPECT_EQ(0, core.o_debug_ppuscroll_y);
    EXPECT_EQ(1, core.o_debug_w);
    
    testBench.tick();
    EXPECT_EQ(kTestX, core.o_debug_ppuscroll_x);
    EXPECT_EQ(kTestY, core.o_debug_ppuscroll_y);
    EXPECT_EQ(0, core.o_debug_w);

    // should not affect other registers
    EXPECT_EQ(0, core.o_debug_ppuctrl);
    EXPECT_EQ(0, core.o_debug_ppumask);
}

TEST_F(PPU, ShouldResetWregWhenReadingFromPPUSTATUS) {
    auto& core = testBench.core();

    // write x ppuscroll, and set W to 1
    const uint kTestX = 42;
    core.i_rs = RS_PPUSCROLL;
    core.i_rw = RW_WRITE;
    core.i_data = kTestX;
    testBench.tick();
    EXPECT_EQ(kTestX, core.o_debug_ppuscroll_x);
    EXPECT_EQ(0, core.o_debug_ppuscroll_y);
    EXPECT_EQ(1, core.o_debug_w);

    // read from PPUSTATUS
    core.i_rs = RS_PPUSTATUS;
    core.i_rw = RW_READ;
    testBench.tick();
    EXPECT_EQ(kTestX, core.o_debug_ppuscroll_x);
    EXPECT_EQ(0, core.o_debug_ppuscroll_y);
    EXPECT_EQ(0, core.o_debug_w);
}

TEST_F(PPU, ShouldWritePPUADDR) {
    auto& core = testBench.core();

    const uint kTestAddress = 0xabcd;

    core.i_rs = RS_PPUADDR;
    core.i_rw = RW_WRITE;
    
    // should only write hi-byte on clock tick
    core.i_data = (kTestAddress >> 8) & 0xff;
    core.eval();
    EXPECT_EQ(0, core.o_debug_ppuaddr);
    EXPECT_EQ(0, core.o_debug_w);
    
    testBench.tick();
    EXPECT_EQ(kTestAddress & 0xff00, core.o_debug_ppuaddr);
    EXPECT_EQ(1, core.o_debug_w);

    // should only write lo-byte on clock tick
    core.i_data = kTestAddress & 0xff;
    core.eval();
    EXPECT_EQ(kTestAddress & 0xff00, core.o_debug_ppuaddr);
    EXPECT_EQ(1, core.o_debug_w);
    
    testBench.tick();
    EXPECT_EQ(kTestAddress, core.o_debug_ppuaddr);
    EXPECT_EQ(0, core.o_debug_w);

    // should not affect other registers
    EXPECT_EQ(0, core.o_debug_ppuctrl);
    EXPECT_EQ(0, core.o_debug_ppumask);
}

TEST_F(PPU, ShouldAccessPaletteDataViaPPUDATA) {
    auto& core = testBench.core();

    const int kNumPaletteEntries = 0x20;

    helperDisableRendering();

    // write to palette data
    core.i_rs = RS_PPUADDR;
    core.i_rw = RW_WRITE;
    core.i_data = 0x3F;
    testBench.tick();
    core.i_data = 0x00;
    testBench.tick();

    core.i_rs = RS_PPUDATA;
    for (int i=0; i<kNumPaletteEntries; i++) {
        core.i_data = i;
        testBench.tick();
    }

    // should not have modified VRAM
    for (size_t i=0; i<vram.size(); i++) {
        EXPECT_EQ(0, vram.read(i));
    }

    // read from palette data
    core.i_rs = RS_PPUADDR;
    core.i_rw = RW_WRITE;
    core.i_data = 0x3F;
    testBench.tick();
    core.i_data = 0x00;
    testBench.tick();

    core.i_rs = RS_PPUDATA;
    core.i_rw = RW_READ;
    core.eval();
    for (int i=0; i<kNumPaletteEntries; i++) {
        EXPECT_EQ(i, core.o_data);
        testBench.tick();
    }
}

TEST_F(PPU, ShouldWriteMirroredPaletteDataViaPPUDATA) {
    auto& core = testBench.core();

    const int kNumPaletteEntries = 0x20;

    helperDisableRendering();

    for (int mirror = 0x20; mirror < 0xff; mirror += 0x20) {
        vram.clear(0);

        // write to palette data
        core.i_rs = RS_PPUADDR;
        core.i_rw = RW_WRITE;
        core.i_data = 0x3F;
        testBench.tick();
        core.i_data = mirror;
        testBench.tick();

        core.i_rs = RS_PPUDATA;
        for (int i=0; i<kNumPaletteEntries; i++) {
            core.i_data = i;
            testBench.tick();
        }

        // should not have modified VRAM
        for (size_t i=0; i<vram.size(); i++) {
            EXPECT_EQ(0, vram.read(i));
        }

        // read from palette data
        core.i_rs = RS_PPUADDR;
        core.i_rw = RW_WRITE;
        core.i_data = 0x3F;
        testBench.tick();
        core.i_data = 0x00;
        testBench.tick();

        core.i_rs = RS_PPUDATA;
        core.i_rw = RW_READ;
        core.eval();
        for (int i=0; i<kNumPaletteEntries; i++) {
            EXPECT_EQ(i, core.o_data);
            testBench.tick();
        }
    }
}

TEST_F(PPU, ShouldReadMirroredPaletteDataViaPPUDATA) {
    auto& core = testBench.core();

    const int kNumPaletteEntries = 0x20;

    helperDisableRendering();

    // write to palette data
    core.i_rs = RS_PPUADDR;
    core.i_rw = RW_WRITE;
    core.i_data = 0x3F;
    testBench.tick();
    core.i_data = 0;
    testBench.tick();

    core.i_rs = RS_PPUDATA;
    for (int i=0; i<kNumPaletteEntries; i++) {
        core.i_data = i;
        testBench.tick();
    }

    for (int mirror = 0x20; mirror < 0xff; mirror += 0x20) {
        // read from palette data
        core.i_rs = RS_PPUADDR;
        core.i_rw = RW_WRITE;
        core.i_data = 0x3F;
        testBench.tick();
        core.i_data = mirror;
        testBench.tick();

        core.i_rs = RS_PPUDATA;
        core.i_rw = RW_READ;
        core.eval();
        for (int i=0; i<kNumPaletteEntries; i++) {
            EXPECT_EQ(i, core.o_data);
            testBench.tick();
        }
    }
}

TEST_F(PPU, ShouldReadVRamViaPPUDATA) {
    auto& core = testBench.core();

    const int kVRamSize = 0x3F00;

    helperDisableRendering();

    // write to VRAM
    for (int i=0; i < kVRamSize; i++) {
        vram.write(i, i % 256);
    }

    // prime PPUADDR to read from VRAM
    core.i_rs = RS_PPUADDR;
    core.i_rw = RW_WRITE;
    core.i_data = 0x00;
    testBench.tick();
    testBench.tick();
    
    // prime internal buffer with the first read
    core.i_rs = RS_PPUDATA;
    core.i_rw = RW_READ;
    testBench.tick();

    // 2nd cycle for internal buffer to be filled from VRAM
    core.i_cs_n = 1;
    testBench.tick();
    core.i_cs_n = 0;

    // TODO: this test only reads up to VRamSize - 1, because the auto-increment will have set addr to start of palette data
    for (int i=0; i<kVRamSize-1; i++) {
        // read from VRAM
        core.eval();

        EXPECT_EQ(i % 256, core.o_data);
        testBench.tick();

        // 2nd cycle for internal buffer to be filled with next read from VRAM
        core.i_cs_n = 1;
        testBench.tick();
        core.i_cs_n = 0;
    }
}

TEST_F(PPU, ShouldWriteVRamViaPPUDATA) {
    auto& core = testBench.core();

    const int kVRamSize = 0x3F00;

    helperDisableRendering();

    // write to VRAM
    core.i_rs = RS_PPUADDR;
    core.i_rw = RW_WRITE;
    core.i_data = 0x00;
    testBench.tick();
    testBench.tick();

    core.i_rs = RS_PPUDATA;
    for (int i=0; i<kVRamSize; i++) {
        // write to VRAM

        // cycle 1 (when write is setup)
        core.i_data = i % 256;
        testBench.tick();

        // cycle 2 (when write occurs)
        core.i_cs_n = 1;
        testBench.tick();
        core.i_cs_n = 0;
    }

    for (int i=0; i<kVRamSize; i++) {
        EXPECT_EQ(i % 256, vram.read(i));
    }
}

TEST_F(PPU, ShouldAccessOAMviaOAMDATA) {
    auto& core = testBench.core();

    const int kOAMSize = 256;

    helperDisableRendering();

    // write to OAM
    core.i_rs = RS_OAMADDR;
    core.i_rw = RW_WRITE;
    core.i_data = 0x00;
    testBench.tick();
    
    // write to OAM
    core.i_rs = RS_OAMDATA;
    for (int i=0; i<kOAMSize; i++) {
        // test auto increment
        EXPECT_EQ(i, core.o_debug_oamaddr);

        // write a byte to oamdata
        core.i_data = i;
        testBench.tick();
    }
    
    // read from oamaddr
    for (int i=0; i<kOAMSize; i++) {
        // set address
        core.i_rs = RS_OAMADDR;
        core.i_rw = RW_WRITE;
        core.i_data = i;
        testBench.tick();

        // read data
        core.i_rs = RS_OAMDATA;
        core.i_rw = RW_READ;
        core.eval();

        EXPECT_EQ(i, core.o_data);
    }
}

TEST_F(PPU, ShouldNotIncrementOAMADDRWhenReadingFromOAMDATA) {
    auto& core = testBench.core();

    helperDisableRendering();

    // write to OAMADDR
    core.i_rs = RS_OAMADDR;
    core.i_rw = RW_WRITE;
    core.i_data = 0x00;
    testBench.tick();
    
    // read from OAMDATA
    core.i_rs = RS_OAMDATA;
    core.i_rw = RW_READ;
    testBench.tick();

    EXPECT_EQ(0x00, core.o_debug_oamaddr);
}

TEST_F(PPU, ShouldAutoIncrementPPUADDRVerticallyWhenReadingFromPPUDATA) {
    auto& core = testBench.core();

    helperDisableRendering();
    helperSetVerticalVRAMIncrement();

    // prime PPUADDR to start reading from 0
    core.i_rs = RS_PPUADDR;
    core.i_rw = RW_WRITE;
    core.i_data = 0x00;
    testBench.tick();
    testBench.tick();
    
    // prime internal buffer with the first read
    core.i_rs = RS_PPUDATA;
    core.i_rw = RW_READ;
    testBench.tick();

    // 2nd cycle for internal buffer to be filled from VRAM
    core.i_cs_n = 1;
    testBench.tick();
    core.i_cs_n = 0;

    EXPECT_EQ(32, core.o_debug_ppuaddr);
}

//
// ppudata
//
// palette
// - reading
//    - internal buffer is filled with data read from elsewhere in VRAM (PPU_ADDR - 0x1000)
//      todo: write test to prime internal buffer like this, and then read it as part of non-palette ppudata read
//

// OAM DMA
// - CPU write to $4014

// rasteriser
// - todo: ...
