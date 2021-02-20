#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "nes/cpu6502/assembler/Assembler.hpp"
#include "nes/cpu6502/assembler/Disassembler.hpp"
#include "nes/memory/SRAM.hpp"
#include "nes/Cpu6502TestBench.h"
#include "nes/emulator/Renderer.hpp"

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

namespace emulator {
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

            // fudge data for reset
            traceOpcode.opcode = 0;
            traceOpcode.addressingMode = 0;
            traceOpcode.labelOpcode = "RESET";
            traceOpcode.labelOperands = "";
            traceOpcode.pc = 0xfffc;
            traceOpcode.byteSize = 0;

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

        Disassembler::DisassembledOpcode traceOpcode;
        
        Renderer renderer;

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

            renderer.drawTitle(*this, 10, 10);
            renderer.drawCPU(*this, 0, 40, testBench);

            renderer.drawDisassembly(*this, 200, 40, disassembledOpcodesAtPC());
            renderer.drawStack(*this, 200, 200, testBench, sram);
            renderer.drawLastOpcodeTrace(*this, 400, 40, testBench.trace, traceOpcode);
        }

        Disassembler::DisassembledOpcodes disassembledOpcodesAtPC() {
            auto& core = testBench.core();
            uint16_t pc = (core.o_debug_pch << 8) + core.o_debug_pcl;
            Disassembler::DisassembledOpcodes disassembledOpcodes = disassembler.disassemble(sram, pc, 10);

            return disassembledOpcodes;
        }

        void simulateOpcode() {
            auto& core = testBench.core();

            // clear trace for the previous opcode
            testBench.trace.clear();

            auto disassembledOpcodes = disassembler.disassemble(sram, (core.o_debug_pch << 8) | core.o_debug_pcl, 1);
            if (disassembledOpcodes.empty()) {
                printf("failed to simulateOpcode()");
                return;
            }

            traceOpcode = disassembledOpcodes[0];
            
            // simulate until next fetching next opcode, or max ticks
            const int kMaxTicks = 10;

            for (int i=0; i<kMaxTicks; i++) {
                testBench.tick();
                if (core.o_sync == 1) {
                    // fetching next opcode
                    break;
                }
            }
        }
    };
}

int main()
{
    emulator::Emulator emulator;

    if (emulator.Construct(kScreenWidth, kScreenHeight, 1, 1))
        emulator.Start();

    return 0;
}
