#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "nes/NESTestBench.h"
#include "nes/memory/SRAM.hpp"

#include <vector>
#include <cassert>

using namespace nestestbench;
using namespace memory;

namespace {
    const uint32_t kScreenWidth = 1000;
    const uint32_t kScreenHeight = 600;

    const int kRowHeight = 11;
}

namespace emulator {
    class EmulatorNES : public olc::PixelGameEngine
    {
    public:
        EmulatorNES() : sram(0x10000), vram(0x10000) {
            sAppName = "Emulator - NES";

            SetPixelMode(olc::Pixel::ALPHA);
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

        // CPU memory
        SRAM sram;

        // PPU memory  
        SRAM vram;

        const int kNESWidth = 341;
        const int kNESHeight = 262;

        std::vector<olc::Pixel> pixels;

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
            const int kNumTicksPerFrame = 2000;       // todo: replace by 30fps timer
            for (int i=0; i<kNumTicksPerFrame; i++) {
                simulateTick();
            }
        }

        void render() {			
            FillRect({ 0,0 }, { ScreenWidth(), ScreenHeight() }, olc::GREY);

            drawTitle(10, 10);
            drawPixels(10, 50);
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

            auto& core = testBench.core();
            FillRect({ x + (core.o_video_x * kPixelSize), y + (core.o_video_y * kPixelSize)}, {kPixelSize << 1, kPixelSize << 1}, olc::WHITE);
        }

        void simulateTick() {
            testBench.tick();

            printf("CPU - IR = 0x%02X\n", testBench.core().o_cpu_debug_ir);
            
            if (testBench.core().o_cpu_debug_error == 1) {
                printf("error!\n");

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

                            printf("write ram 0x%04X = 0x%02X\n", core.o_address_ram, core.o_data_ram);
                        } else {
                            core.i_data_ram = sram.read(core.o_address_ram);

                            printf("read ram 0x%04X = 0x%02X\n", core.o_address_ram, core.i_data_ram);
                        }
                    }
                    
                    if (core.o_cs_prg == 1) {
                        core.i_data_prg = sram.read(core.o_address_prg);

                        printf("read prg 0x%04X = 0x%02X\n", core.o_address_prg, core.i_data_prg);
                    }

                    if (core.o_cs_patterntable == 1) {
                        if (core.o_rw_patterntable == 1) {
                            core.i_data_patterntable = vram.read(core.o_address_patterntable);

                            printf("read patterntable 0x%04X\n = 0x%02X", core.o_address_patterntable, core.i_data_patterntable);
                        } else {
                            printf("???? write patterntable 0x%04X ???? - not supported!\n", core.o_address_patterntable);
                            exit(2);
                        }
                    }

                    if (core.o_cs_nametable == 1) {
                        if (core.o_rw_nametable == 0) {                            
                            vram.write(core.o_address_nametable, core.o_data_nametable);

                            printf("write nametable 0x%04X = 0x%02X\n", core.o_address_nametable, core.o_data_nametable);
                        } else {
                            core.i_data_nametable = vram.read(core.o_address_nametable);

                            printf("read nametable 0x%04X = 0x%02X\n", core.o_address_nametable, core.i_data_nametable);
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
