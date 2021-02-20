#include "nes/emulator/Renderer.hpp"
#include "nes/cpu6502/ProcessorStatusFlags.hpp"

using namespace cpu6502;

namespace {
    std::string PrepareString(const char* format, ...) {        
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);        
        va_end(args);
        
        std::string ret = buffer;

        return ret;
    }
}

namespace emulator {
    void Renderer::drawTitle(olc::PixelGameEngine& engine, int x, int y) {
        engine.DrawString({x,y}, "Nintendo ENTERTAINMENT SYSTEM fpga version", olc::RED);
        y += 10;
        engine.DrawLine({x, y}, {x + 42 * 8, y}, olc::RED);
    }

    void Renderer::drawCPU(olc::PixelGameEngine& engine, int x, int y, cpu6502testbench::Cpu6502TestBench& testBench) {
        engine.DrawString({ x, y }, "CPU State", olc::RED);

        auto& core = testBench.core();

        uint8_t p = core.o_debug_p;

        std::vector<std::string> reports = {
            PrepareString("tick: %d", int(testBench.stepCount() / 2)),
            PrepareString("  ac: 0x%02x", core.o_debug_ac),
            PrepareString("   x: 0x%02x", core.o_debug_x),
            PrepareString("   y: 0x%02x", core.o_debug_y),
            PrepareString("   s: 0x01%02x", core.o_debug_s),
            PrepareString("  pc: 0x%02x%02x", core.o_debug_pch, core.o_debug_pcl),
            PrepareString(" p.C: %d", (p & C) ? 1 : 0),
            PrepareString(" p.Z: %d", (p & Z) ? 1 : 0),
            PrepareString(" p.I: %d", (p & I) ? 1 : 0),
            PrepareString(" p.D: %d", (p & D) ? 1 : 0),
            PrepareString(" p.B: %d", (p & B) ? 1 : 0),
            PrepareString(" p.V: %d", (p & V) ? 1 : 0),
            PrepareString(" p.N: %d", (p & N) ? 1 : 0)
        };

        y += 10;
        for (auto it = reports.begin(); it != reports.end(); it++) {
            engine.DrawString({ x + 10, y }, *it, olc::BLACK);
            y += 10;
        }
    }

    void Renderer::drawDisassembly(olc::PixelGameEngine& engine, int x, int y, const cpu6502::assembler::Disassembler::DisassembledOpcodes& opcodes) {
        engine.DrawString({ x, y }, "Disassembly", olc::RED);
        
        y += 10;

        engine.DrawString({ x, y}, ">", olc::RED);
    
        for (auto& opcode: opcodes) {
            
            std::string strOpcode = opcode.labelOpcode + " " + opcode.labelOperands;
            
            engine.DrawString({ x + 10, y }, PrepareString("0x%04x %s", opcode.pc, strOpcode.c_str()), olc::BLACK);
            y += 10;
        }
    }

    void Renderer::drawStack(olc::PixelGameEngine& engine, int x, int y, cpu6502testbench::Cpu6502TestBench& testBench, const memory::SRAM& sram) {
        engine.DrawString({ x, y }, "Stack", olc::RED);

        const int kOffset = 3;

        uint8_t s = testBench.core().o_debug_s;

        s -= kOffset;

        y += 10;
        for (int i = 0; i < 10; i++) {
            uint16_t address = 0x0100 + s;
            uint8_t data = sram.read(address);

            engine.DrawString({ x + 10, y }, PrepareString("0x%04x %02x", address, data), olc::BLACK);
            if (i == kOffset) {
                engine.DrawString({ x, y}, ">", olc::RED);
            }

            y += 10;
            s += 1;
        }
    }

    void Renderer::drawLastOpcodeTrace(olc::PixelGameEngine& engine, int x, int y, const gtestverilog::Trace& trace, const cpu6502::assembler::Disassembler::DisassembledOpcode& opcode) {
        engine.DrawString({ x, y }, "Last Opcode", olc::RED);

        y += 10;

        int numTicks = int(trace.getSteps().size() / 2);
        engine.DrawString({x, y}, PrepareString("%d clock cycles", numTicks), olc::BLACK);
        y += 10;
        engine.DrawString({x, y}, PrepareString("0x%04x:  %s %6s    # 0x%02x, %d bytes", 
                                        opcode.pc,
                                        opcode.labelOpcode.c_str(),
                                        opcode.labelOperands.c_str(),
                                        opcode.opcode,
                                        int(opcode.byteSize)
                                        ), olc::BLACK);
        y+= 10;

        //testBench.trace;

        // note: each text character is 8x8 pixels

        // todo: render it !
        
        
    }
}
