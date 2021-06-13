#include <numeric>

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/NESTestBench.h"
using namespace nestestbench;

namespace {
    class NES : public ::testing::Test {
    public:
        NES() {
            
        }

        void SetUp() override {
            testBench.setClockPolarity(1);
            
            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {

        }

        NESTestBench testBench;
    };
}

TEST_F(NES, ShouldConstruct) {

}
