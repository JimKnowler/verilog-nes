#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/ALUTestBench.h"
using namespace alutestbench;

namespace {
    class ALU : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        ALUTestBench testBench;
    };
}

TEST_F(ALU, ShouldConstruct) {
}

TEST_F(ALU, ShouldReset) {
    testBench.reset();
}

