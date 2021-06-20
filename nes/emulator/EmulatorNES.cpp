#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "nes/NESTestBench.h"

#include <vector>
#include <cassert>

using namespace nestestbench;

namespace {
    const uint32_t kScreenWidth = 1000;
    const uint32_t kScreenHeight = 600;

    const int kRowHeight = 11;
}

namespace emulator {
    class EmulatorNES : public olc::PixelGameEngine
    {
    public:
        EmulatorNES() {
            sAppName = "Emulator - NES";

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
        NESTestBench testBench;

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
            DrawString({x,y}, "NES Output Example", olc::RED);
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
            FillRect({ x + (core.o_video_x * kPixelSize), y + (core.o_video_y * kPixelSize)}, {kPixelSize, kPixelSize}, olc::WHITE);
        }

        void simulateTick() {
            testBench.tick();

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
    };
}

int main()
{
    emulator::EmulatorNES emulator;

    if (emulator.Construct(kScreenWidth, kScreenHeight, 1, 1))
        emulator.Start();

    return 0;
}
