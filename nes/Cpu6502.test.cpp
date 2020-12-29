#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/Cpu6502TestBench.h"
using namespace cpu6502testbench;

#include "cpu6502/opcode/Assembler.h"
using namespace cpu6502::opcode;

namespace {
    class Cpu6502 : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        Cpu6502TestBench testBench;
    };
}

TEST_F(Cpu6502, ShouldConstruct) {
}

TEST_F(Cpu6502, ShouldReset) {
    testBench.reset();

    // todo: assert state of output ports
}