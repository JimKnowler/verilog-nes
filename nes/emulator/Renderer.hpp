#pragma once

#include "olcPixelGameEngine.h"

#include "nes/Cpu6502TestBench.h"
#include "nes/cpu6502/assembler/Disassembler.hpp"

namespace emulator {

    class Renderer {
    public:
        void drawTitle(olc::PixelGameEngine& engine, int x, int y);
        void drawCPU(olc::PixelGameEngine& engine, int x, int y, cpu6502testbench::Cpu6502TestBench& testBench);
        void drawDisassembly(olc::PixelGameEngine& engine, int x, int y, const cpu6502::assembler::Disassembler::DisassembledOpcodes& opcodes);
        void drawStack(olc::PixelGameEngine& engine, int x, int y, cpu6502testbench::Cpu6502TestBench& testBench, const memory::SRAM& sram);
        void drawLastOpcodeTrace(olc::PixelGameEngine& engine, int x, int y, const gtestverilog::Trace& trace, const cpu6502::assembler::Disassembler::DisassembledOpcode& opcode);

    private:
    };

}
