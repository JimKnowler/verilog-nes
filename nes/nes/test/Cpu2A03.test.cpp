#include <numeric>

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/Cpu2A03TestBench.h"
using namespace cpu2a03testbench;

namespace {
    class Cpu2A03 : public ::testing::Test {
    public:

        void SetUp() override {
            testBench.setClockPolarity(1);
            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {

        }

        Cpu2A03TestBench testBench;
    };
}

TEST_F(Cpu2A03, ShouldConstruct) {

}