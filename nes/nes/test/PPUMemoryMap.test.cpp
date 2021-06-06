#include <numeric>

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PPUMemoryMapTestBench.h"
using namespace ppumemorymaptestbench;


namespace {
    class PPUMemoryMap : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
            testBench.reset();
        }

        PPUMemoryMapTestBench testBench;
    };
}

TEST_F(PPUMemoryMap, ShouldConstruct) {
    // testing that test fixture can safely construct and reset
}

// mirror palette 0x3000/0x3FFF to 0x2000/0x2FFF
// mirror nametables (0x2000/0x2FFF) based on h/v mirroring 
//   -> including mirror of palette

// input i_nametable_mirroring : 0 = horizontal, 1 = vertical
// memory port - CHR ROM 0x000 -> 0x1FFF
//   - address, chip select, data (read)
// memory port - nametable 0x2000 -> 0x2FFF
//   - address, chip select, RW, data (read), data (write)

// NAMETABLE VRAM
// output - chip select
// output read/write
// input/output data
