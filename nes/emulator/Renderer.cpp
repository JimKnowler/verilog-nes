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

    const std::vector<olc::Pixel> kColours = {
        olc::RED,
        olc::GREEN,
        olc::YELLOW,
        olc::DARK_CYAN,
        olc::MAGENTA,
        olc::CYAN,
        olc::WHITE
    };

    olc::Pixel getColourForPortId(uint32_t id) {
        return kColours[ id % kColours.size() ];
    }

    const int kCharWidth = 8;
    const int kRowHeight = 11;

    const int kStepWidth = 16;
    const int kHalfStepWidth = kStepWidth / 2;
}

namespace emulator {
    void Renderer::drawTitle(olc::PixelGameEngine& engine, int x, int y) {
        engine.DrawString({x,y}, "Nintendo ENTERTAINMENT SYSTEM fpga version", olc::RED);
        y += kRowHeight;

        engine.DrawLine({x, y}, {x + 42 * 8, y}, olc::RED);
    }

    void Renderer::drawCPU(olc::PixelGameEngine& engine, int x, int y, cpu6502testbench::Cpu6502TestBench& testBench) {
        engine.DrawString({ x, y }, "CPU State", olc::RED);
        y += kRowHeight;

        auto& core = testBench.core();
        uint8_t p = core.o_debug_p;

        std::vector<std::string> registers = {
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

        for (auto it = registers.begin(); it != registers.end(); it++) {
            engine.DrawString({ x + 10, y }, *it, olc::BLACK);
            y += kRowHeight;
        }
    }

    void Renderer::drawTestBench(olc::PixelGameEngine& engine, int x, int y, cpu6502testbench::Cpu6502TestBench& testBench, int numOpcodes) {
        engine.DrawString({ x, y }, "TestBench", olc::RED);
        y += kRowHeight;

        auto& core = testBench.core();

        bool hasErrored = (core.o_debug_error == 1);
        engine.DrawString({ x + 10, y }, "   state: ", olc::BLACK);
        engine.DrawString({x + 10 + (kCharWidth*10), y}, (hasErrored) ? "ERROR" : "OK", (hasErrored) ? olc::RED : olc::GREEN);
        y += kRowHeight;

        std::vector<std::string> registers = {
            PrepareString("    tick: %d", int(testBench.stepCount() / 2)),
            PrepareString(" opcodes: %d", numOpcodes)
        };

        for (auto it = registers.begin(); it != registers.end(); it++) {
            engine.DrawString({ x + 10, y }, *it, olc::BLACK);
            y += kRowHeight;
        }
    }

    void Renderer::drawDisassembly(olc::PixelGameEngine& engine, int x, int y, const cpu6502::assembler::Disassembler::DisassembledOpcodes& opcodes) {
        engine.DrawString({ x, y }, "Disassembly", olc::RED);
        y += kRowHeight;

        engine.DrawString({ x, y}, ">", olc::RED);
    
        for (auto& opcode: opcodes) {
            
            std::string strOpcode = opcode.labelOpcode + " " + opcode.labelOperands;
            
            engine.DrawString({ x + 10, y }, PrepareString("0x%04x %s", opcode.pc, strOpcode.c_str()), olc::BLACK);
            y += kRowHeight;
        }
    }

    void Renderer::drawStack(olc::PixelGameEngine& engine, int x, int y, cpu6502testbench::Cpu6502TestBench& testBench, const memory::SRAM& sram) {
        engine.DrawString({ x, y }, "Stack", olc::RED);

        const int kOffset = 3;

        uint8_t s = testBench.core().o_debug_s;

        s -= kOffset;

        y += kRowHeight;
        for (int i = 0; i < 10; i++) {
            uint16_t address = 0x0100 + s;
            uint8_t data = sram.read(address);

            engine.DrawString({ x + 10, y }, PrepareString("0x%04x %02x", address, data), olc::BLACK);
            if (i == kOffset) {
                engine.DrawString({ x, y}, ">", olc::RED);
            }

            y += kRowHeight;
            s += 1;
        }
    }

    void Renderer::drawLastOpcodeTrace(olc::PixelGameEngine& engine, int x, int y, const gtestverilog::Trace& trace, const cpu6502::assembler::Disassembler::DisassembledOpcode& opcode) {
        engine.DrawString({ x, y }, "Last Opcode", olc::RED);
        y += kRowHeight;

        int numTicks = int(trace.getSteps().size() / 2);
        engine.DrawString({x, y}, PrepareString("%d clock cycles", numTicks), olc::BLACK);
        y += kRowHeight;
        engine.DrawString({x, y}, PrepareString("0x%04x:  %-16s # 0x%02x, %d bytes", 
                                        opcode.pc,
                                        (opcode.labelOpcode + " " + opcode.labelOperands).c_str(),
                                        opcode.opcode,
                                        int(opcode.byteSize)
                                        ), olc::BLACK);

        y += kRowHeight;
        for (size_t i=0; i < opcode.data.size(); i++) {
            engine.DrawString({x + (9 * kCharWidth) + int(i * 5 * kCharWidth), y}, PrepareString("0x%02X", opcode.data[i]), olc::BLACK);
        }

        y+= (2 * kRowHeight);

        drawTrace(engine, x, y, trace);
    }

    
    void Renderer::drawTrace(olc::PixelGameEngine& engine, int x, int y, const gtestverilog::Trace& trace) {
        // note: each text character is 8x8 pixels       
        // note: each Row is 10 pixels high

        auto& steps = trace.getSteps();
        
        size_t maxPortLabelSize = trace.getMaxPortLabelSize();

        const int yTimeline = y;
        y += kRowHeight;

        const int yStart = y;

        int portIndex = 0;
        for (uint32_t portId=0; portId<32; portId++) {
            if (!trace.hasPort(portId)) {
                continue;
            }

            const gtestverilog::PortDescription& portDesc = trace.getPortDescription(portId);

            size_t numRows = drawTracePort(engine, x, y, portIndex, maxPortLabelSize, portDesc, steps);
            y += (numRows * kRowHeight);
            portIndex += 1;
        }

        const int yEnd = y;

        drawTraceTimeline(engine, x, yTimeline, maxPortLabelSize + 11, steps.size(), yStart, yEnd);
    }

    void Renderer::drawTraceTimeline(olc::PixelGameEngine& engine, int x, int y, size_t offsetX, size_t numSteps, int yTraceStart, int yTraceEnd) {
        x += (offsetX * kCharWidth);

        const int kDividerSize = 2;
        int numDividers = (numSteps + kDividerSize - 1) / kDividerSize;

        for (int i=0; i<numDividers;i++) {
            engine.DrawLine({x,y}, {x,y+kRowHeight}, olc::BLACK);
            engine.DrawString({x + 4,y}, PrepareString("%d", (i * kDividerSize) / 2), olc::BLACK);

            engine.DrawLine({x,yTraceStart}, {x,yTraceEnd}, olc::Pixel(150,150,150,50));

            x += (kDividerSize * kStepWidth);
        }

        engine.DrawLine({x,yTraceStart}, {x,yTraceEnd}, olc::Pixel(150,150,150,50));
    }

    size_t Renderer::drawTracePort(olc::PixelGameEngine& engine, int x, int y, int portIndex, size_t maxPortLabelSize, const gtestverilog::PortDescription& portDesc, const std::vector<gtestverilog::Step>& steps) {
        size_t numRows = size_t(ceil(portDesc.width() / 4.0f));

        // choose colour for the port
        const olc::Pixel kColour = getColourForPortId(portDesc.id());

        // fill background for the port
        engine.FillRect({x,y}, {x + 100, int(numRows * kRowHeight)}, ((portIndex % 2) == 1) ? olc::GREY : olc::Pixel(200, 200, 200));

        // print name of the row
        engine.DrawString({x,y}, PrepareString("  %*s ", int(maxPortLabelSize), portDesc.label()), kColour);
        x += ( (maxPortLabelSize + 3) * kCharWidth);

        // print bit ranges for each row
        for (size_t row=0; row < numRows; row++) {
            if (portDesc.width() > 1) {
                size_t nibbleIndex = numRows - row - 1;
                engine.DrawString({x,y + int(row * kRowHeight)}, PrepareString("[%02lu:%02lu] ", ((nibbleIndex + 1) * 4) - 1, (nibbleIndex * 4)), kColour);                
            } else {
                engine.DrawString({x,y}, ".......", kColour);
            }
        }

        x += (8 * kCharWidth);

        if (steps.size() > 0) {
            // draw logic analyser diagram

            if (portDesc.width() == 1) {
                // single bit
                bool lastBit = std::get<bool>(steps[0].port(portDesc));

                const int kLogicHeight = kRowHeight - 2;

                for (size_t i=0; i<steps.size(); i++) {
                    bool bit = std::get<bool>(steps[i].port(portDesc));

                    int bitY = y + (bit ? 2 : kLogicHeight);

                    if (bit != lastBit) {
                        // render edge
                        int lastBitY = y + (lastBit ? 2 : kLogicHeight);

                        engine.DrawLine({x,lastBitY}, {x,bitY}, kColour);
                    }

                    engine.DrawLine({x,bitY}, {x+kStepWidth,bitY}, kColour);
                    
                    lastBit = bit;
                    x += kStepWidth;
                }
            } else {
                // multibit

                size_t stepIndex = 0;
                const int kPortHeight = numRows * kRowHeight;
                const int kHalfPortHeight = kPortHeight / 2;
                while (stepIndex < steps.size()) {
                    uint32_t value = std::get<uint32_t>(steps[stepIndex].port(portDesc));

                    size_t runLength = 1;
                    stepIndex += 1;

                    while ((stepIndex < steps.size()) && (value == std::get<uint32_t>(steps[stepIndex].port(portDesc)))) {
                        runLength += 1;
                        stepIndex += 1;
                    }
                    
                    // open edge
                    engine.FillTriangle({x,y + kHalfPortHeight}, {x + kHalfStepWidth, y+kPortHeight}, {x + kHalfStepWidth, y}, kColour);
                    x += kHalfStepWidth;

                    // filled area
                    if (runLength > 1) {
                        const int kFillWidth = int((runLength-1) * kStepWidth);
                        engine.FillRect({x,y}, {kFillWidth, kPortHeight}, kColour);

                        for (size_t row=0; row < numRows; row++) {
                            size_t nibbleIndex = numRows - row - 1;
                            
                            uint8_t nibble = uint8_t( (value >> (nibbleIndex * 4)) & 0xf);
                            engine.DrawString({1 + x, 1 + y + int(row * kRowHeight)}, PrepareString("%X", nibble), olc::BLACK);
                        }
                        x += kFillWidth;
                    }
                    
                    // closing edge
                    engine.FillTriangle({x + kHalfStepWidth, y + kHalfPortHeight}, {x, y + kPortHeight}, {x, y}, kColour);
                    x += kHalfStepWidth; 
                }
            }
        }

        return numRows;
    }
}
