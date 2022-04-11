#include <numeric>

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/CPUMemoryMapTestBench.h"
using namespace cpumemorymaptestbench;

const int RW_READ = 1;
const int RW_WRITE = 0;

const int ADDRESS_RAM_START = 0x0000;
const int ADDRESS_RAM_END = 0x07FF;

const int ADDRESS_RAM_MIRROR_START = 0x0800;
const int ADDRESS_RAM_MIRROR_END = 0x1FFF;

const int ADDRESS_PPU_START = 0x2000;
const int ADDRESS_PPU_END = 0x2007;

const int ADDRESS_PPU_MIRROR_START = 0x2008;
const int ADDRESS_PPU_MIRROR_END = 0x3FFF;

const int ADDRESS_PRG_START = 0x8000;
const int ADDRESS_PRG_END = 0xFFFF;

namespace {
    class CPUMemoryMap : public ::testing::Test {
    public:
        void SetUp() override {
            auto& core = testBench.core();
            core.i_clk_en = 1;
            core.i_oe1_n = 1;
            core.i_oe2_n = 1;

            testBench.setClockPolarity(1);
            testBench.reset();
        }

        CPUMemoryMapTestBench testBench;
    };
}

TEST_F(CPUMemoryMap, ShouldConstruct) {
    // testing that test fixture can safely construct and reset
}

TEST_F(CPUMemoryMap, ShouldReadFromRAM) {
    auto& core = testBench.core();
    
    for (int address=ADDRESS_RAM_START, i=0; address<=ADDRESS_RAM_END; address++, i++) {
        int data = i % 256;
        core.i_address_cpu = address;
        core.i_rw_cpu = RW_READ;
        core.i_data_ram = data;
        core.eval();

        EXPECT_EQ(RW_READ, core.o_rw_ram);
        EXPECT_EQ(address, core.o_address_ram);

        EXPECT_EQ(0, core.o_cs_prg);
        EXPECT_EQ(1, core.o_cs_ram);
        EXPECT_EQ(data, core.o_data_cpu);
    }
}

TEST_F(CPUMemoryMap, ShouldWriteToRAM) {
    auto& core = testBench.core();
    
    for (int address=ADDRESS_RAM_START, i=0; address<=ADDRESS_RAM_END; address++, i++) {
        int data = i % 256;
        core.i_address_cpu = address;
        core.i_rw_cpu = RW_WRITE;
        core.i_data_cpu = data;
        core.eval();

        EXPECT_EQ(RW_WRITE, core.o_rw_ram);
        EXPECT_EQ(address, core.o_address_ram);

        EXPECT_EQ(0, core.o_cs_prg);
        EXPECT_EQ(1, core.o_cs_ram);
        EXPECT_EQ(data, core.o_data_ram);
    }
}

TEST_F(CPUMemoryMap, ShouldReadFromMirroredRAM) {
    auto& core = testBench.core();
    
    for (int address=ADDRESS_RAM_MIRROR_START, i=0; address<=ADDRESS_RAM_MIRROR_END; address++, i++) {
        int data = i % 256;
        core.i_address_cpu = address;
        core.i_rw_cpu = RW_READ;
        core.i_data_ram = data;
        core.eval();

        EXPECT_EQ(RW_READ, core.o_rw_ram);
        EXPECT_EQ(address % 0x0800, core.o_address_ram);
        
        EXPECT_EQ(0, core.o_cs_prg);
        EXPECT_EQ(1, core.o_cs_ram);
        EXPECT_EQ(data, core.o_data_cpu);
    }
}

TEST_F(CPUMemoryMap, ShouldWriteToMirroredRAM) {
    auto& core = testBench.core();
    
    for (int address=ADDRESS_RAM_MIRROR_START, i=0; address<=ADDRESS_RAM_MIRROR_END; address++, i++) {
        int data = i % 256;
        core.i_address_cpu = address;
        core.i_rw_cpu = RW_WRITE;
        core.i_data_cpu = data;
        core.eval();

        EXPECT_EQ(RW_WRITE, core.o_rw_ram);
        EXPECT_EQ(address % 0x0800, core.o_address_ram);
        
        EXPECT_EQ(0, core.o_cs_prg);
        EXPECT_EQ(1, core.o_cs_ram);
        EXPECT_EQ(data, core.o_data_ram);
    }
}

TEST_F(CPUMemoryMap, ShouldReadFromPRGROM) {
    auto& core = testBench.core();
    
    for (int address=ADDRESS_PRG_START, i=0; address<=ADDRESS_PRG_END; address++, i++) {
        int data = i % 256;
        core.i_address_cpu = address;
        core.i_rw_cpu = RW_READ;
        core.i_data_prg = data;
        core.eval();

        EXPECT_EQ(address, core.o_address_prg);
        EXPECT_EQ(1, core.o_cs_prg);
        EXPECT_EQ(0, core.o_cs_ram);
        EXPECT_EQ(data, core.o_data_cpu);
    }
}

TEST_F(CPUMemoryMap, ShouldReadFromPPU) {
    auto& core = testBench.core();
    
    for (int address=ADDRESS_PPU_START, i=0; address<=ADDRESS_PPU_END; address++, i++) {
        int data = i % 256;
        core.i_address_cpu = address;
        core.i_rw_cpu = RW_READ;
        core.i_data_ppu = data;
        core.eval();

        EXPECT_EQ(RW_READ, core.o_rw_ppu);
        EXPECT_EQ(address & 7, core.o_rs_ppu); // 3 least significant bits
        
        EXPECT_EQ(0, core.o_cs_prg);
        EXPECT_EQ(0, core.o_cs_ram);
        EXPECT_EQ(data, core.o_data_cpu);
    }
}

TEST_F(CPUMemoryMap, ShouldWriteToPPU) {
    auto& core = testBench.core();
    
    for (int address=ADDRESS_PPU_START, i=0; address<=ADDRESS_PPU_END; address++, i++) {
        int data = i % 256;
        core.i_address_cpu = address;
        core.i_rw_cpu = RW_WRITE;
        core.i_data_cpu = data;
        core.eval();

        EXPECT_EQ(RW_WRITE, core.o_rw_ppu);
        EXPECT_EQ(address & 7, core.o_rs_ppu); // 3 least significant bits
        
        EXPECT_EQ(0, core.o_cs_prg);
        EXPECT_EQ(0, core.o_cs_ram);
        EXPECT_EQ(data, core.o_data_ppu);
    }
}

TEST_F(CPUMemoryMap, ShouldReadFromMirroredPPU) {
    auto& core = testBench.core();
    
    for (int address=ADDRESS_PPU_MIRROR_START, i=0; address<=ADDRESS_PPU_MIRROR_END; address++, i++) {
        int data = i % 256;
        core.i_address_cpu = address;
        core.i_rw_cpu = RW_READ;
        core.i_data_ppu = data;
        core.eval();

        EXPECT_EQ(RW_READ, core.o_rw_ppu);
        EXPECT_EQ(address & 7, core.o_rs_ppu); // 3 least significant bits
        
        EXPECT_EQ(0, core.o_cs_prg);
        EXPECT_EQ(0, core.o_cs_ram);
        EXPECT_EQ(data, core.o_data_cpu);
    }
}

TEST_F(CPUMemoryMap, ShouldWriteToMirroredPPU) {
    auto& core = testBench.core();
    
    for (int address=ADDRESS_PPU_MIRROR_START, i=0; address<=ADDRESS_PPU_MIRROR_END; address++, i++) {
        int data = i % 256;
        core.i_address_cpu = address;
        core.i_rw_cpu = RW_WRITE;
        core.i_data_cpu = data;
        core.eval();

        EXPECT_EQ(RW_WRITE, core.o_rw_ppu);
        EXPECT_EQ(address & 7, core.o_rs_ppu); // 3 least significant bits
        
        EXPECT_EQ(0, core.o_cs_prg);
        EXPECT_EQ(0, core.o_cs_ram);
        EXPECT_EQ(data, core.o_data_ppu);
    }
}

TEST_F(CPUMemoryMap, ShouldOnlyEnableChipSelectsWhenClockIsEnabled) {
    auto& core = testBench.core();
    
    for (int address=0; address<=0xFFFF; address++) {
        core.i_clk_en = 0;
        core.i_address_cpu = address;
        core.eval();
        
        EXPECT_EQ(0, core.o_cs_prg);
        EXPECT_EQ(0, core.o_cs_ram);
    }
}

TEST_F(CPUMemoryMap, ShouldReadFromController1) {
    auto& core = testBench.core();
    core.i_oe1_n = 0;
    core.i_address_cpu = 0x4016;            // Register JOY1
    core.i_rw_cpu = RW_READ;

    core.i_controller_1 = 0;
    core.eval();
    EXPECT_EQ(0x41, core.o_data_cpu);      // read inverted value from controller 1, combined with open bus (0x40)
    EXPECT_EQ(0, core.o_cs_prg);
    EXPECT_EQ(0, core.o_cs_ram);

    core.i_controller_1 = 1;
    core.eval();
    EXPECT_EQ(0x40, core.o_data_cpu);      // read inverted value from controller 1, combined with open bus (0x40)
    EXPECT_EQ(0, core.o_cs_prg);
    EXPECT_EQ(0, core.o_cs_ram);
}

// TODO: cartridge space for mappers at 0x4020
//      https://wiki.nesdev.com/w/index.php/CPU_memory_map