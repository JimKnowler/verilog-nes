#pragma once

#include "olcPixelGameEngine.h"

#include "nes/Cpu6502TestBench.h"
#include "nes/cpu6502/assembler/Disassembler.hpp"

namespace emulator {

    class RendererCPU {
    public:
        void drawTitle(olc::PixelGameEngine& engine, int x, int y);
        void drawCPU(olc::PixelGameEngine& engine, int x, int y, cpu6502testbench::Cpu6502TestBench& testBench);
        void drawTestBench(olc::PixelGameEngine& engine, int x, int y, cpu6502testbench::Cpu6502TestBench& testBench, int numOpcodes);
        void drawDisassembly(olc::PixelGameEngine& engine, int x, int y, const cpu6502::assembler::Disassembler::DisassembledOpcodes& opcodes);
        void drawStack(olc::PixelGameEngine& engine, int x, int y, cpu6502testbench::Cpu6502TestBench& testBench, const memory::SRAM& sram);
        void drawLastOpcodeTrace(olc::PixelGameEngine& engine, int x, int y, const gtestverilog::Trace& trace, const cpu6502::assembler::Disassembler::DisassembledOpcode& opcode);

    private:
        void drawTrace(olc::PixelGameEngine& engine, int x, int y, const gtestverilog::Trace& trace);
        void drawTraceTimeline(olc::PixelGameEngine& engine, int x, int y, size_t offsetX, size_t numSteps, int yTraceStart, int yTraceEnd);
        size_t drawTracePort(olc::PixelGameEngine& engine, int x, int y, int portIndex, size_t maxPortLabelSize, const gtestverilog::PortDescription& portDesc, const std::vector<gtestverilog::Step>& steps);  
    };

}
