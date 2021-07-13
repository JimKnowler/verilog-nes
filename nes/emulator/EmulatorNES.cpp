#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "nes/NESTestBench.h"
#include "nes/memory/SRAM.hpp"

#include <vector>
#include <cassert>

using namespace nestestbench;
using namespace memory;

#define LOG_CPU(msg, ...)   printf(msg, __VA_ARGS__) 
#define LOG_BUS(msg, ...)   printf(msg, __VA_ARGS__)

namespace {
    const uint32_t kScreenWidth = 1300;
    const uint32_t kScreenHeight = 700;

    const int kRowHeight = 11;
}

namespace emulator {
    class EmulatorNES : public olc::PixelGameEngine
    {
    public:
        EmulatorNES() : sram(0x10000), vram(0x10000) {
            sAppName = "Emulator - NES";
        }

        /// @brief called once at start		
        bool OnUserCreate() override {
            initSimulation();

            initMario();

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
        NESTestBench testBench;
        int numTicks = 0;
        int numFrames = 0;

        // CPU memory
        SRAM sram;

        // PPU memory  
        SRAM vram;

        const int kNESWidth = 341;
        const int kNESHeight = 262;

        std::vector<olc::Pixel> pixels;

        enum Mode {
            kSingleStep,
            kRun
        };

        Mode mode = kRun;

        int vramDisplay = 0;

        void reset() {
            testBench.reset();

            resetPixels();            
        }

        void resetPixels() {
            // reset pixel buffer
            pixels.resize( kNESWidth * kNESHeight );
            std::fill(pixels.begin(), pixels.end(), olc::YELLOW);

            // fill current pixel
            writeCurrentPixel();
        }

        void update() {
            if (GetKey(olc::V).bReleased) {
                toggleDisplayVRAM();
            }

            switch (mode) {
                case kRun:
                {
                    // todo: replace by 30fps timer
                    const int kNumTicksPerFrame = 2000;
                    
                    for (int i=0; i<kNumTicksPerFrame; i++) {
                        simulateTick();
                    }

                    if(GetKey(olc::R).bReleased) {
                        mode = kSingleStep;
                    }

                    break;
                }
                case kSingleStep:
                {
                    if (GetKey(olc::SPACE).bReleased) {
                        int numTicks = 1;

                        if (GetKey(olc::RIGHT).bHeld) {
                            numTicks = 100;
                        }

                        for (int i=0; i<numTicks; i++) {
                            simulateTick();
                        }
                    }

                    if (GetKey(olc::R).bReleased) {
                        mode = kRun;
                    }

                    break;
                }
                default:
                {
                    break;
                }
            }

            
        }

        void render() {			
            FillRect({ 0,0 }, { ScreenWidth(), ScreenHeight() }, olc::GREY);

            drawTitle(10, 10);
            drawPixels(10, 50);
            drawStats(700, 50);

            // visual debug of VRAM
            const int kVramX = 700;
            const int kVramY = 350;

            switch (vramDisplay) {
                case 0:
                    drawNametable(kVramX, kVramY);
                    break;
                case 1:
                    drawPatternTable(kVramX, kVramY);
                    break;                
                default:
                   break;
            }
        }

        void toggleDisplayVRAM() {
            vramDisplay = (vramDisplay + 1) % 2;
        }

        std::string bitLabel(uint8_t value, const std::string& label) {
            std::string output;

            for (int i=0; i<8; i++) {
                if ((value & (1<<(7-i))) != 0) {
                    output += label[i];
                } else {
                    output += '.';
                }
            }

            return output;
        }

        void drawStats(int x, int y) {
            DrawString({x,y}, "Stats", olc::RED);
            y += kRowHeight;
            DrawLine({x, y}, {x + 42 * 8, y}, olc::RED);
            y += kRowHeight;

            char buffer[32];
            sprintf(buffer, "     ticks %d", numTicks);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;

            sprintf(buffer, "     frame %d", numFrames);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;
            y += kRowHeight;

            // CPU
            DrawString({x,y}, "CPU", olc::RED);
            y += kRowHeight;
            DrawLine({x, y}, {x + 42 * 8, y}, olc::RED);
            y += kRowHeight;

            sprintf(buffer, "    clk-en %d", testBench.core().o_cpu_debug_clk_en);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;

            sprintf(buffer, "        ir 0x%02x", testBench.core().o_cpu_debug_ir);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;

            sprintf(buffer, "       tcu 0x%02x", testBench.core().o_cpu_debug_tcu);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;

            sprintf(buffer, "   address 0x%04x", testBench.core().o_cpu_debug_address);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;  
            y += kRowHeight;

            // PPU
            DrawString({x,y}, "PPU", olc::RED);
            y += kRowHeight;
            DrawLine({x, y}, {x + 42 * 8, y}, olc::RED);
            y += kRowHeight;

            sprintf(buffer, "         x %3d", testBench.core().o_video_x);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;

            sprintf(buffer, "         y %3d", testBench.core().o_video_y);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;

            uint8_t ppuctrl = testBench.core().o_ppu_debug_ppuctrl;
            sprintf(buffer, "   ppuctrl 0x%02x %s", ppuctrl, bitLabel(ppuctrl, "VPHBSINN").c_str());
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;

            uint8_t ppumask = testBench.core().o_ppu_debug_ppumask;
            sprintf(buffer, "   ppumask 0x%02x %s", ppumask, bitLabel(ppumask, "BGRsbMmG").c_str());
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;

            uint8_t ppustatus = testBench.core().o_ppu_debug_ppustatus;
            sprintf(buffer, " ppustatus 0x%02x %s", ppustatus, bitLabel(ppustatus, "VSO.....").c_str());
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;

            uint16_t vramAddress = testBench.core().o_ppu_debug_vram_address;
            sprintf(buffer, " vram addr 0x%04x", vramAddress);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;
            
            sprintf(buffer, "  scroll-x %03d", testBench.core().o_ppu_debug_ppuscroll_x);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;

            sprintf(buffer, "  scroll-y %03d", testBench.core().o_ppu_debug_ppuscroll_y);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight;  

            sprintf(buffer, "         v 0x%04x", testBench.core().o_ppu_debug_v);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight; 

            sprintf(buffer, "         t 0x%04x", testBench.core().o_ppu_debug_t);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight; 

            sprintf(buffer, "         x %d", testBench.core().o_ppu_debug_x);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight; 

            sprintf(buffer, "         w %d", testBench.core().o_ppu_debug_w);
            DrawString({x,y}, buffer, olc::BLACK);
            y += kRowHeight; 
        }

        void drawTitle(int x, int y) {
            DrawString({x,y}, "FPGA - NES Emulator", olc::RED);
            y += kRowHeight;

            DrawLine({x, y}, {x + 42 * 8, y}, olc::RED);
        }

        void drawPixels(int x, int y) {
            const int kPixelSize = 2;

            for (int col = 0; col<kNESWidth; col++) {
                for (int row = 0; row<kNESHeight; row++) {
                    const olc::Pixel pixel = pixels[col + (row*kNESWidth)];
                    FillRect({ x + (col*kPixelSize), y + (row*kPixelSize)}, {kPixelSize, kPixelSize}, pixel);
                }
            }

            // current output position
            auto& core = testBench.core();
            FillRect({ x , y + (core.o_video_y*kPixelSize)}, {kPixelSize * kNESWidth, kPixelSize}, olc::GREY);
            FillRect({ x + (core.o_video_x * kPixelSize), y + (core.o_video_y * kPixelSize)}, {kPixelSize << 1, kPixelSize << 1}, olc::WHITE);
        }

        void drawPatternTable(int x, int y) {
            DrawString({x,y}, "VRAM - Pattern Table", olc::RED);
            y += kRowHeight;
            DrawLine({x, y}, {x + 42 * 8, y}, olc::RED);
            y += kRowHeight;

            for (int section=0; section<2; section++) {
                for (int c = 0; c<16; c++) {
                    for (int r =0; r<16; r++) {
                        // position of character's top left corner
                        int tx = x + (section * 128) + (c*8);
                        int ty = y + (r * 8);

                        drawCharacter(tx, ty, section, c, r);
                    }
                }
            }
        }

        void drawNametable(int x, int y) {
            DrawString({x,y}, "VRAM - Nametable", olc::RED);
            y += kRowHeight;
            DrawLine({x, y}, {x + 42 * 8, y}, olc::RED);
            y += kRowHeight;

            for (int section=0; section<2; section++) {
                for (int c = 0; c<32; c++) {
                    for (int r =0; r<30; r++) {
                        // read value from nametable
                        uint8_t tile = vram.read(0x2000 + (section * 0x0800) + c + (r * 32));

                        // position of character's top left corner
                        int tx = x + (section * 32 * 8) + (c*8);
                        int ty = y + (r * 8);
                        
                        int patternTableSection = 1;        // TODO: read from PPUCTRL[4]
                        drawCharacter(tx, ty, patternTableSection, tile & 0xF, (tile >> 4) & 0xF);
                    }
                }
            }
        }

        void drawCharacter(int x, int y, int section, int c, int r) {
            for (int i=0; i<8; i++) {
                // each row in the character
                uint8_t low = vram.read(i | (c<<4) | (r<<8) | (section << 12));
                uint8_t high = vram.read(i | (1<<3) | (c<<4) | (r<<8) | (section << 12));

                for (int p=0; p<8; p++) {
                    // each pixel in the row
                    uint8_t lowBit = (low >> (7-p)) & 0x1;
                    uint8_t highBit = (high >> (7-p)) & 0x1;
                    uint8_t colour = lowBit + (highBit << 1);

                    const static olc::Pixel kColours[] = { olc::BLACK, olc::DARK_GREY, olc::GREY, olc::WHITE };

                    DrawRect({x+p, y+i}, {1,1}, kColours[colour]);
                }
            }
        }

        void simulateTick() {
            auto& core = testBench.core();

            testBench.tick();

            if ((core.o_video_x == 0) && (core.o_video_y == 0)) {
                numFrames +=1;
            }
            numTicks += 1;

            // only log this on ticks when CPU clock enable is active
            LOG_CPU("CPU - IR:0x%02X address:0x%04X rw:%d\n", core.o_cpu_debug_ir, core.o_cpu_debug_address, core.o_cpu_debug_rw);
            
            if (core.o_cpu_debug_error == 1) {
                printf("error! tick (%d) frame (%d)\n", numTicks, numFrames);

                exit(2);
            }

            writeCurrentPixel();
        }

        void writeCurrentPixel() {
            auto& core = testBench.core();  

            int x = core.o_video_x;
            int y = core.o_video_y;
            int red = core.o_video_red;
            int green = core.o_video_green;
            int blue = core.o_video_blue;


            olc::Pixel pixel(red, green, blue);
            pixels[x + (y*kNESWidth)] = pixel;
        }

        void initSimulation() {
            testBench.setClockPolarity(0);

            sram.clear(0);
            vram.clear(0);

            // simulation at the end of a clock phase, before
            //   transition to other clock phase
            testBench.setCallbackSimulateCombinatorial([this]{
                auto& core = testBench.core();

                if (core.i_clk == 1) {
                    // clock: end of phi2
                    // R/W data is valid on the bus

                    if (core.o_cs_ram == 1) {
                        if (core.o_rw_ram == 0) {
                            sram.write(core.o_address_ram, core.o_data_ram);

                            LOG_BUS("write ram 0x%04X = 0x%02X\n", core.o_address_ram, core.o_data_ram);
                        } else {
                            core.i_data_ram = sram.read(core.o_address_ram);

                            LOG_BUS("read ram 0x%04X = 0x%02X\n", core.o_address_ram, core.i_data_ram);
                        }
                    }
                    
                    if (core.o_cs_prg == 1) {
                        core.i_data_prg = sram.read(core.o_address_prg);

                        LOG_BUS("read prg 0x%04X = 0x%02X\n", core.o_address_prg, core.i_data_prg);
                    }

                    if (core.o_cs_patterntable == 1) {
                        if (core.o_rw_patterntable == 1) {
                            core.i_data_patterntable = vram.read(core.o_address_patterntable);

                            LOG_BUS("read patterntable 0x%04X\n = 0x%02X", core.o_address_patterntable, core.i_data_patterntable);
                        } else {
                            LOG_BUS("???? write patterntable 0x%04X ???? - not supported!\n", core.o_address_patterntable);
                            exit(2);
                        }
                    }

                    if (core.o_cs_nametable == 1) {
                        if (core.o_rw_nametable == 0) {                            
                            vram.write(core.o_address_nametable, core.o_data_nametable);

                            LOG_BUS("write nametable 0x%04X = 0x%02X\n", core.o_address_nametable, core.o_data_nametable);
                        } else {
                            core.i_data_nametable = vram.read(core.o_address_nametable);

                            LOG_BUS("read nametable 0x%04X = 0x%02X\n", core.o_address_nametable, core.i_data_nametable);
                        }
                    }

                } else {
                    // clock: end of phi 1
                    // undefined data on the bus
                    core.i_data_ram = 0xFF;
                    core.i_data_prg = 0xFF;
                    core.i_data_patterntable = 0xFF;
                    core.i_data_nametable = 0xFF;
                }
            });
        }

        void initMario() {
            // PRG - game code

            // load bank 0 -> 0x8000:0xBFFF
            auto bank0 = loadBinaryFile("roms/supermario/prg_rom_bank_0.6502.bin");
            sram.write(0x8000, bank0);

            // load bank 1 -> 0xC000:0xFFFF
            auto bank1 = loadBinaryFile("roms/supermario/prg_rom_bank_1.6502.bin");
            sram.write(0xC000, bank1);

            // CHR - pattern table
            auto chr = loadBinaryFile("roms/supermario/chr_rom_bank_0.bin");
            vram.write(0x0000, chr);
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
    };
}

int main()
{
    emulator::EmulatorNES emulator;

    if (emulator.Construct(kScreenWidth, kScreenHeight, 1, 1))
        emulator.Start();

    return 0;
}
