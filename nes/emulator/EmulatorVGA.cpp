#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "nes/VGAExampleTestBench.h"

#include <vector>
#include <cassert>

using namespace vgaexampletestbench;

namespace {
    const uint32_t kScreenWidth = 1000;
    const uint32_t kScreenHeight = 600;

    //const int kCharWidth = 8;
    const int kRowHeight = 11;

    //const int kStepWidth = 16;
    //const int kHalfStepWidth = kStepWidth / 2;
}

namespace emulator {
    class EmulatorVGA : public olc::PixelGameEngine
    {
    public:
        EmulatorVGA() {
            sAppName = "Emulator - VGA Example";

            SetPixelMode(olc::Pixel::ALPHA);
        }

        /// @brief called once at start		
        bool OnUserCreate() override {
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
        VGAExampleTestBench testBench;

        const int kVGAWidth = 360;
        const int kVGAHeight = 260;

        std::vector<olc::Pixel> pixels;

        void reset() {
            testBench.reset();

            resetRect();
            resetPixels();            
        }

        void resetPixels() {
            // reset pixel buffer
            pixels.resize( kVGAWidth * kVGAHeight );
            std::fill(pixels.begin(), pixels.end(), olc::YELLOW);

            // fill current pixel
            writeCurrentPixel();
        }

        void resetRect() {
            auto& core = testBench.core();

            // determine dimensions of the rectangle that the simulation will render
            core.i_rect_x = 10;
            core.i_rect_y = 15;
            core.i_rect_width = 80;
            core.i_rect_height = 66;
            core.eval();
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
            DrawString({x,y}, "VGA Output Example", olc::RED);
            y += kRowHeight;

            DrawLine({x, y}, {x + 42 * 8, y}, olc::RED);
        }

        void drawPixels(int x, int y) {
            const int kPixelSize = 2;

            for (int col = 0; col<kVGAWidth; col++) {
                for (int row = 0; row<kVGAHeight; row++) {
                    const olc::Pixel pixel = pixels[col + (row*kVGAWidth)];
                    FillRect({ x + (col*kPixelSize), y + (row*kPixelSize)}, {kPixelSize, kPixelSize}, pixel);
                }
            }

            // todo: render electron-gun position
        }

        void simulateTick() {
            testBench.tick();

            writeCurrentPixel();
        }

        void writeCurrentPixel() {
            auto& core = testBench.core();  

            int x = core.o_vga_x;
            int y = core.o_vga_y;
            int red = core.o_vga_red;
            int green = core.o_vga_green;
            int blue = core.o_vga_blue;

            if (core.o_vga_hsync == 0) {
                blue += 0x90;
            }

            if (core.o_vga_vsync == 0) {
                red += 0x90;
            }

            olc::Pixel pixel(red, green, blue);
            pixels[x + (y*kVGAWidth)] = pixel;
        }
    };
}

int main()
{
    emulator::EmulatorVGA emulator;

    if (emulator.Construct(kScreenWidth, kScreenHeight, 1, 1))
        emulator.Start();

    return 0;
}
