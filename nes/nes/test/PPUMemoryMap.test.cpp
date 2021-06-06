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
        }

        PPUMemoryMapTestBench testBench;
    };
}

TEST_F(PPUMemoryMap, ShouldConstruct) {
    
}