#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "nes/cpu6502/assembler/Assembler.hpp"
#include "nes/cpu6502/assembler/Disassembler.hpp"
#include "nes/memory/SRAM.hpp"

#include <vector>
#include <cassert>
#include <chrono>

using namespace cpu6502::assembler;

namespace {
    const uint32_t kScreenWidth = 1000;
    const uint32_t kScreenHeight = 600;
}

class Emulator : public olc::PixelGameEngine
{
public:
    Emulator() {
        sAppName = "FGPA NES Emulator";
    }

	/// @brief called once at start		
    bool OnUserCreate() override {
        
        return true;
    }

	/// @brief called every frame
    bool OnUserUpdate(float fElapsedTime) override {
			
        FillRect({ 0,0 }, { ScreenWidth(), ScreenHeight() }, olc::GREY);

/*
		DrawString({ 10,10 }, PrepareString("Mode [%s]", (mode == Mode::Debugger) ? "DEBUGGER" : "RUN"));
        DrawCPU(10,40);
        DrawOpcodes(200,40);
        DrawMemory("HL", emulator.getState().hl, 10, 200);
        DrawMemory("DE", emulator.getState().de, 10, 300);
        DrawStack(200, 200);
*/		

        return true;
    }

	void updateFrame() {

	}


private:
    Disassembler disassembler;

    /*
    void DrawCPU(int x, int y) {
        DrawString({ x, y }, "CPU State");

        const cpu::State& state = emulator.getState();
        uint64_t numSteps = emulator.getNumSteps();

        std::vector<std::string> reports = {
            PrepareString("step: %llu", numSteps),
            PrepareString("   a: 0x%02x", state.a),
            PrepareString("  bc: 0x%02x%02x", state.b, state.c),
            PrepareString("  de: 0x%02x%02x", state.d, state.e),
            PrepareString("  hl: 0x%02x%02x", state.h, state.l),
            PrepareString("  pc: 0x%04x", state.pc),
            PrepareString("  sp: 0x%04x", state.sp),
            PrepareString("   z: %u", state.cc.z),
            PrepareString("   s: %u", state.cc.s),
            PrepareString("   p: %u", state.cc.p),
            PrepareString("  cy: %u", state.cc.cy),
            PrepareString("  ac: %u", state.cc.ac),
        };

        y += 10;
        for (auto it = reports.begin(); it != reports.end(); it++) {
            DrawString({ x + 10, y }, *it);
            y += 10;
        }

    }

    void DrawOpcodes(int x, int y) {
        DrawString({ x, y }, "Opcodes");
        
        const cpu::State& state = emulator.getState();
        uint16_t pc = state.pc;

        y += 10;
        for (int i = 0; i < 10; i++) {
			uint16_t opcodeSize;			
			std::string strOpcode = Disassemble::stringFromOpcode(&memory, pc, opcodeSize);
            
            DrawString({ x + 10, y }, PrepareString("0x%04x %s", pc, strOpcode.c_str()));
            y += 10;

            pc += opcodeSize;
        }
    }

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

    void DrawStack(int x, int y) {
        DrawString({ x, y }, "Stack");

        const cpu::State& state = emulator.getState();
        uint16_t sp = state.sp;
        uint16_t address = sp & ~3;

        y += 10;
        for (int i = 0; i < 10; i++) {
			if (address >= memory.size()) {
				break;
			}
            DrawString({ x + 10, y }, PrepareString("0x%04x %02x %02x %02x %02x", address, memory.read(address), memory.read(address + 1), memory.read(address + 2), memory.read(address + 3)));

            y += 10;
            address += 4;
        }
    }

	void DrawVideoRam(int x, int y) {
		const uint16_t kVideoRamStart = 0x2400;

		int i = 0;
		const int kVideoRamWidth = 224;
		const int kVideoRamHeight = 256;

		for (int ix = 0; ix < kVideoRamWidth; ix++) {
			for (int iy = 0; iy < kVideoRamHeight; iy +=8) {
				uint8_t byte = memory.read(kVideoRamStart + i++);

				for (int b = 0; b < 8; b++) {
					FillRect({ x + ( ix << 1), y + ((kVideoRamHeight - (iy + b)) << 1) }, { 2,2 }, ((byte & 0x1) == 0x1) ? olc::WHITE : olc::BLACK);
					byte >>= 1;
				}
			}
		}
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

	std::chrono::time_point<std::chrono::system_clock> getCurrentTime() const {
		return std::chrono::system_clock::now();
	}
    */

};

int main()
{
    Emulator emulator;

    if (emulator.Construct(kScreenWidth, kScreenHeight, 1, 1))
        emulator.Start();

    return 0;
}
