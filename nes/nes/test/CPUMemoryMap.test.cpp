#include <numeric>

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/CPUMemoryMapTestBench.h"
using namespace cpumemorymaptestbench;

namespace {
    class CPUMemoryMap : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
        }

        CPUMemoryMapTestBench testBench;
    };
}

TEST_F(CPUMemoryMap, ShouldConstruct) {

}