#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "nes/cpu6502/assembler/Assembler.hpp"
#include "nes/cpu6502/assembler/Disassembler.hpp"
#include "nes/memory/SRAM.hpp"
#include "nes/Cpu6502TestBench.h"
#include "nes/emulator/RendererCPU.hpp"

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
        Emulator() : sram(0x10000) {
            sAppName = "Emulator - CPU 6502";

            SetPixelMode(olc::Pixel::ALPHA);
        }

        /// @brief called once at start		
        bool OnUserCreate() override {
            initSimulation();

            //initSimpleProgram();
            //initMario(); 
            initNesTest();

            mode = kSingleStep;

            reset();

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

        Disassembler::DisassembledOpcode lastOpcode;
        gtestverilog::Trace traceLastOpcode;
        
        RendererCPU renderer;

        int numOpcodes;

        enum Mode {
            kSingleStep,
            kRun
        };

        Mode mode;

        void reset() {
            testBench.reset();

            // todo: reset testBench step count
            simulateOpcode();               // skip SYNC incorrectly reported during RESET
            
            // skip reset
            simulateOpcode();

            // fudge data for reset
            lastOpcode.opcode = 0;
            lastOpcode.addressingMode = 0;
            lastOpcode.labelOpcode = "RESET";
            lastOpcode.labelOperands = "";
            lastOpcode.pc = 0xfffc;
            lastOpcode.byteSize = 0;

            numOpcodes = 0;
        }

        void initMario() {
            // load bank 0 -> 0x8000:0xBFFF
            auto bank0 = loadBinaryFile("roms/supermario/prg_rom_bank_0.6502.bin");
            sram.write(0x8000, bank0);

            // load bank 1 -> 0xC000:0xFFFF
            auto bank1 = loadBinaryFile("roms/supermario/prg_rom_bank_1.6502.bin");
            sram.write(0xC000, bank1);

            // simulate PPUSTATUS register
            // https://wiki.nesdev.com/w/index.php/PPU_registers#Status_.28.242002.29_.3C_read
            // note: simulating VBLANK has started
            sram.write(0x2002, 0x80);
        }

        void initNesTest() {
            // load bank 0 -> 0xC000:0xFFFF
            auto bank1 = loadBinaryFile("roms/nestest/prg_rom_bank_0.6502.bin");
            sram.write(0xC000, bank1);

            // start PC at 0xc000
            sram.write(0xfffc, 0x00);       // low byte
            sram.write(0xfffd, 0xc0);       // high byte
        }

        std::vector<uint8_t> loadBinaryFile(const char* filename) {
            std::ifstream is;
            is.open (filename, std::ios::binary );
            assert(is.is_open());

            is.seekg (0, std::ios::end);
            int length = is.tellg();
            is.seekg (0, std::ios::beg);
            std::vector<uint8_t> buffer(length);
            uint8_t* pBuffer = &(buffer.front());
            is.read(reinterpret_cast<char*>(pBuffer), length);
            is.close();

            return buffer;
        }

        void initSimpleProgram() {
            // create a simple program & compile it into SRAM

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
        }

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
        }

        void update() {
            if(GetKey(olc::ESCAPE).bReleased) {
                exit(0);
            }

            if(GetKey(olc::Z).bReleased) {
                mode = kSingleStep;
                reset();
            }

            if(GetKey(olc::R).bReleased) {
                if (mode == kRun) {
                    mode = kSingleStep;
                } else {
                    mode = kRun;
                }
            }

            switch (mode) {
                case kSingleStep:
                if (GetKey(olc::SPACE).bReleased) {
                    simulateOpcode();
                }
                break;
                case kRun:
                {
                    const int kNumOpcodesPerFrame = 100;
                    
                    for (int i=0; i < kNumOpcodesPerFrame; i++) {
                        if (hasCoreErrored())
                        {
                            mode = kSingleStep;
                            break;
                        } else {
                            simulateOpcode();

                            // hard coded breakpoint
                            /*if (lastOpcode.pc == 0xCF83) {
                                mode = kSingleStep;
                                break;
                            }
                            */

                            /*
                            if (numOpcodes == 1045) {
                                mode = kSingleStep;
                                break;
                            }
                            */
                        }                        
                    }
                
                    break;
                }
                default:
                break;
            }
        }

        void render() {			
            FillRect({ 0,0 }, { ScreenWidth(), ScreenHeight() }, olc::GREY);

            renderer.drawTitle(*this, 10, 10);
            renderer.drawCPU(*this, 10, 40, testBench);
            renderer.drawTestBench(*this, 10, 200, testBench, numOpcodes);
            renderer.drawMemory(*this, 10, 250, sram);

            renderer.drawDisassembly(*this, 200, 40, disassembledOpcodesAtPC());
            renderer.drawStack(*this, 200, 200, testBench, sram);
            renderer.drawLastOpcodeTrace(*this, 400, 40, traceLastOpcode, lastOpcode);
        }

        Disassembler::DisassembledOpcodes disassembledOpcodesAtPC() {
            auto& core = testBench.core();
            uint16_t pc = (core.o_debug_pch << 8) + core.o_debug_pcl;
            Disassembler::DisassembledOpcodes disassembledOpcodes = disassembler.disassemble(sram, pc, 10);

            return disassembledOpcodes;
        }

        bool hasCoreErrored() {
            auto& core = testBench.core();

            return core.o_debug_error == 1;
        }

        uint16_t getCorePC() {
            auto& core = testBench.core();
            
            return (core.o_debug_pch << 8) | core.o_debug_pcl;
        }

        void simulateOpcode() {
            if (hasCoreErrored()) {
                // core has errored, so we can't simluate any further
                return;
            }

            // clear trace for the previous opcode
            gtestverilog::Trace lastTrace = testBench.trace;
            testBench.trace.clear();

            uint16_t pc = getCorePC();
            auto disassembledOpcodes = disassembler.disassemble(sram, pc, 1);
            if (disassembledOpcodes.empty()) {
                printf("failed to simulateOpcode()");
                return;
            }

            lastOpcode = disassembledOpcodes[0];
            
            // simulate until next fetching next opcode, or max ticks
            const int kMaxTicks = 10;

            auto& core = testBench.core();

            core.i_clk_en = 1;
            core.i_irq_n = 1;
            core.i_nmi_n = 1;

            for (int i=0; i<kMaxTicks; i++) {
                testBench.tick();
                if (core.o_sync == 1) {
                    // opcode has completed, and we are now fetching next opcode
                    numOpcodes += 1;
                    break;
                }

                if (hasCoreErrored()) {
                    // core has errored.. so tick once more (for debug display) then stop
                    testBench.tick();
                    break;
                }
            }

            // we've stopped half with through T0 of next opcode, so we need to 
            // concatenate part of the last trace with the new trace in order
            // to create a full trace for last opcode
            traceLastOpcode = lastTrace.slice( lastTrace.getSteps().size() - 1, 1) + testBench.trace.slice(0, testBench.trace.getSteps().size()-1);

            if (lastOpcode.opcode != 0) {
                logLastOpcode();
            }
        }

        void logLastOpcode() {
            printf("%04X  ", lastOpcode.pc);
            for (auto byte: lastOpcode.data) {
                printf("%02X ", byte);
            }

            for (size_t i=lastOpcode.data.size(); i<3; i++) {
                printf("   ");
            }
            printf(" ");

            Disassembler::DisassembledOpcodes disassembledOpcodes = disassembler.disassemble(sram, lastOpcode.pc, 1);
            Disassembler::DisassembledOpcode disassembledOpcode = disassembledOpcodes[0];
            
            char buffer[64];
            sprintf(buffer, "%s %s", disassembledOpcode.labelOpcode.c_str(), disassembledOpcode.labelOperands.c_str());

            for (size_t i=strlen(buffer); i<32; i++) {
                buffer[i] = ' ';
            }

            buffer[32] = 0;
            printf("%s", buffer);

            // note: some opcodes are still writing to registers in the first clock cycle
            const gtestverilog::Step& step = traceLastOpcode.getSteps()[2];

            printf("A:%02X X:%02X Y:%02X P:%02X SP:%02X", 
                    uint8_t(std::get<uint32_t>(step.port(o_debug_ac))),
                    uint8_t(std::get<uint32_t>(step.port(o_debug_x))),
                    uint8_t(std::get<uint32_t>(step.port(o_debug_y))),
                    uint8_t(std::get<uint32_t>(step.port(o_debug_p))),
                    uint8_t(std::get<uint32_t>(step.port(o_debug_s)))
                );

            printf("\n");
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
