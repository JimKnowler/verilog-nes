#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "nes/cpu6502/assembler/Assembler.hpp"
#include "nes/cpu6502/assembler/Disassembler.hpp"
#include "nes/cpu6502/ProcessorStatusFlags.hpp"
#include "nes/memory/SRAM.hpp"
#include "nes/Cpu6502TestBench.h"

#include <vector>
#include <cassert>

using namespace cpu6502;
using namespace cpu6502::assembler;
using namespace memory;
using namespace cpu6502testbench;

namespace {
    const uint32_t kScreenWidth = 1000;
    const uint32_t kScreenHeight = 600;
}

class Emulator : public olc::PixelGameEngine
{
public:
    Emulator() : sram(0xffff) {
        sAppName = "FGPA NES Emulator";
    }

	/// @brief called once at start		
    bool OnUserCreate() override {
        initSimulation();

        // create a simple program

        Assembler()
                .NOP()
            .org(0x3456)
            .label("start")
                .NOP()
                .JSR().absolute("jmp_to")
            .label("return_to")
                .SEC()
                .CLC()
                .LDY().immediate(0x80)          // N
                .LDX().immediate(0)             // Z
                .TAX()
                .TAY()
                .CMP().immediate(0x42)
            .org(0x4567)
            .label("jmp_to")
                .NOP()
                .LDA().immediate(0x42)
                .RTS()
            .org(0xfffc)
            .word("start")
            .compileTo(sram);

        // skip reset
        simulateOpcode();

        return true;
    }

	/// @brief called every frame
    bool OnUserUpdate(float fElapsedTime) override {
        update();

        render();

        return true;
    }
    
private:
    Disassembler disassembler;

    Cpu6502TestBench testBench;
    SRAM sram;
    
    void initSimulation() {
        testBench.setClockPolarity(0);

        sram.clear(0);

        // simulation at the end of a clock phase, before
        //   transition to other clock phase
        testBench.setCallbackSimulateCombinatorial([this]{
            auto& cpu = testBench.core();

            if (cpu.i_clk == 1) {
                // clock: end of phi2
                // R/W data is valid on the bus
                if (cpu.o_rw == 0) {
                    // write
                    sram.write(cpu.o_address, cpu.o_data);
                } else {
                    // read
                    cpu.i_data = sram.read(cpu.o_address);
                }

    #ifdef CPU6502_VERBOSE
                printf("Cpu6502: %s addr [0x%04x] data [0x%02x]\n", 
                        (cpu.o_rw == 1) ? "R" : "W",
                        cpu.o_address,
                        sram.read(cpu.o_address));
    #endif
            } else {
                // clock: end of phi 1
                // undefined data on the bus
                cpu.i_data = 0xFF;
            }
        });

        testBench.reset();
        simulateOpcode();               // incorrectly reporting SYNC during RESET?
    }

    void update() {
        if (GetKey(olc::SPACE).bReleased) {
			simulateOpcode();
		}
    }

    void render() {			
        FillRect({ 0,0 }, { ScreenWidth(), ScreenHeight() }, olc::GREY);

        DrawString({10,10}, "Nintendo ENTERTAINMENT SYSTEM fpga version", olc::RED);
        DrawLine({10,20}, {10 + 42 * 8, 20}, olc::RED);

        DrawCPU(10,40);
        DrawDisassembly(200,40);
        DrawStack(200, 200);
        DrawTrace(400, 40);
    }

    void simulateOpcode() {
        // clear trace for the previous opcode
        testBench.trace.clear();

        // simulate until next fetching next opcode, or max ticks
        const int kMaxTicks = 10;

        for (int i=0; i<kMaxTicks; i++) {
            testBench.tick();
            if (testBench.core().o_sync == 1) {
                // fetching next opcode
                break;
            }
        }
    }

    void DrawCPU(int x, int y) {
        DrawString({ x, y }, "CPU State", olc::RED);

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
            DrawString({ x + 10, y }, *it, olc::BLACK);
            y += 10;
        }
    }

    void DrawDisassembly(int x, int y) {
        DrawString({ x, y }, "Disassembly", olc::RED);
        
        auto& core = testBench.core();
        uint16_t pc = (core.o_debug_pch << 8) + core.o_debug_pcl;

        Disassembler::DisassembledOpcodes opcodes = disassembler.disassemble(sram, pc, 10);

        y += 10;
        for (auto& opcode: opcodes) {
			
			std::string strOpcode = opcode.labelOpcode + " " + opcode.labelOperands;
            
            DrawString({ x + 10, y }, PrepareString("0x%04x %s", opcode.pc, strOpcode.c_str()), olc::BLACK);
            y += 10;
        }
    }

    /*

    void DrawMemory(const char* label, uint16_t address, int x, int y) {
        DrawString({ x, y }, PrepareString("Memory (%s)", label));

        // 4 byte alignment
        address &= ~3;

        if (address < memory.size()) {
            y += 10;
            for (int i = 0; i < 8; i++) {
                DrawString({ x + 10, y }, PrepareString("0x%04x %02x %02x %02x %02x", address, memory.read(address), memory.read(address + 1), memory.read(address + 2), memory.read(address + 3)));

                y += 10;
                address += 4;
            }
        }
    }
    */

    void DrawStack(int x, int y) {
        DrawString({ x, y }, "Stack", olc::RED);

        const int kOffset = 3;

        uint8_t s = testBench.core().o_debug_s;

        s -= kOffset;

        y += 10;
        for (int i = 0; i < 10; i++) {
            uint16_t address = 0x0100 + s;
			uint8_t data = sram.read(address);

            DrawString({ x + 10, y }, PrepareString("0x%04x %02x", address, data), olc::BLACK);
            if (i == kOffset) {
                DrawString({ x, y}, ">", olc::RED);
            }

            y += 10;
            s += 1;
        }
    }

    void DrawTrace(int x, int y) {
        DrawString({ x, y }, "Trace", olc::RED);

        const auto& trace = testBench.trace;

        y += 10;

        int numTicks = int(trace.getSteps().size() / 2);
        DrawString({x, y}, PrepareString("%d clock cycles", numTicks), olc::BLACK);

        std::ostringstream streamTrace;
        streamTrace << testBench.trace;
    
        std::string strTrace = streamTrace.str();
        
        y+= 10;
        DrawString({x, y}, strTrace, olc::BLACK);
        DrawString({x+(4*8), y+(5*8)}, "X", olc::RED);      // each character is 8x8 pixels

        // escape sequence is \e[<x>;<y>m]
    }

    std::string PrepareString(const char* format, ...) {        
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);        
        va_end(args);
        
        std::string ret = buffer;

        return ret;
    }
};

int main()
{
    Emulator emulator;

    if (emulator.Construct(kScreenWidth, kScreenHeight, 1, 1))
        emulator.Start();

    return 0;
}
