#include <numeric>

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUMemoryMapTestBench.h"
using namespace ppumemorymaptestbench;

const int RW_READ = 1;
const int RW_WRITE = 0;

const int ADDRESS_PATTERNTABLE_START = 0x0000;
const int ADDRESS_PATTERNTABLE_END = 0x1FFF;

const int ADDRESS_NAMETABLE_START = 0x2000;
const int ADDRESS_NAMETABLE_END = 0x2FFF;

const int ADDRESS_NAMETABLE_MIRROR_START = 0x3000;
const int ADDRESS_NAMETABLE_MIRROR_END = 0x3FFF;

namespace {
    class PPUMemoryMap : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
            testBench.reset();
        }

        PPUMemoryMapTestBench testBench;
    };
}

TEST_F(PPUMemoryMap, ShouldConstruct) {
    // testing that test fixture can safely construct and reset
}

TEST_F(PPUMemoryMap, ShouldReadFromPatternTables) {
    auto& core = testBench.core();

    for (int address=ADDRESS_PATTERNTABLE_START, i=0; address<=ADDRESS_PATTERNTABLE_END; address++, i++) {
        int data = i % 256;
        core.i_address_ppu = address;
        core.i_rd_en_ppu_n = 0;
        core.i_wr_en_ppu_n = 1;
        core.i_data_patterntable = data;
        core.eval();

        EXPECT_EQ(RW_READ, core.o_rw_patterntable);
        EXPECT_EQ(address, core.o_address_patterntable);

        EXPECT_EQ(1, core.o_cs_patterntable);
        EXPECT_EQ(0, core.o_cs_nametable);
        EXPECT_EQ(data, core.o_data_ppu);
    }
}

TEST_F(PPUMemoryMap, ShouldReadFromNametable) {
    auto& core = testBench.core();

    for (int address=ADDRESS_NAMETABLE_START, i=0; address<=ADDRESS_NAMETABLE_END; address++, i++) {
        int data = i % 256;
        core.i_address_ppu = address;
        core.i_rd_en_ppu_n = 0;
        core.i_wr_en_ppu_n = 1;
        core.i_data_nametable = data;
        core.eval();

        EXPECT_EQ(RW_READ, core.o_rw_nametable);
        EXPECT_EQ(address, core.o_address_nametable);

        EXPECT_EQ(0, core.o_cs_patterntable);
        EXPECT_EQ(1, core.o_cs_nametable);
        EXPECT_EQ(data, core.o_data_ppu);
    }
}

TEST_F(PPUMemoryMap, ShouldWriteToNametable) {
    auto& core = testBench.core();

    for (int address=ADDRESS_NAMETABLE_START, i=0; address<=ADDRESS_NAMETABLE_END; address++, i++) {
        int data = i % 256;
        core.i_address_ppu = address;
        core.i_rd_en_ppu_n = 1;
        core.i_wr_en_ppu_n = 0;
        core.i_data_ppu = data;
        core.eval();

        EXPECT_EQ(RW_WRITE, core.o_rw_nametable);
        EXPECT_EQ(address, core.o_address_nametable);

        EXPECT_EQ(0, core.o_cs_patterntable);
        EXPECT_EQ(1, core.o_cs_nametable);
        EXPECT_EQ(data, core.o_data_nametable);
    }
}

TEST_F(PPUMemoryMap, ShouldReadFromNametableMirror) {
    auto& core = testBench.core();

    for (int address=ADDRESS_NAMETABLE_MIRROR_START, i=0; address<=ADDRESS_NAMETABLE_MIRROR_END; address++, i++) {
        int data = i % 256;
        core.i_address_ppu = address;
        core.i_rd_en_ppu_n = 0;
        core.i_wr_en_ppu_n = 1;
        core.i_data_nametable = data;
        core.eval();

        EXPECT_EQ(RW_READ, core.o_rw_nametable);
        EXPECT_EQ(address - 0x1000, core.o_address_nametable);

        EXPECT_EQ(0, core.o_cs_patterntable);
        EXPECT_EQ(1, core.o_cs_nametable);
        EXPECT_EQ(data, core.o_data_ppu);
    }
}

TEST_F(PPUMemoryMap, ShouldWriteToNametableMirror) {
    auto& core = testBench.core();

    for (int address=ADDRESS_NAMETABLE_MIRROR_START, i=0; address<=ADDRESS_NAMETABLE_MIRROR_END; address++, i++) {
        int data = i % 256;
        core.i_address_ppu = address;
        core.i_rd_en_ppu_n = 1;
        core.i_wr_en_ppu_n = 0;
        core.i_data_ppu = data;
        core.eval();

        EXPECT_EQ(RW_WRITE, core.o_rw_nametable);
        EXPECT_EQ(address - 0x1000, core.o_address_nametable);

        EXPECT_EQ(0, core.o_cs_patterntable);
        EXPECT_EQ(1, core.o_cs_nametable);
        EXPECT_EQ(data, core.o_data_nametable);
    }
}

TEST_F(PPUMemoryMap, ShouldNotEnableChipSelectOutputWhenPPUIsNotReadingOrWriting) {
    auto& core = testBench.core();

    for (int address=ADDRESS_PATTERNTABLE_START, i=0; address<=ADDRESS_NAMETABLE_MIRROR_END; address++, i++) {
        core.i_address_ppu = address;
        core.i_rd_en_ppu_n = 1;
        core.i_wr_en_ppu_n = 1;
        core.eval();

        EXPECT_EQ(0, core.o_cs_patterntable);
        EXPECT_EQ(0, core.o_cs_nametable);
    }
}
