#pragma once

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/Cpu6502TestBench.h"
using namespace cpu6502testbench;

#include "cpu6502/opcode/Opcodes.h"
using namespace cpu6502::opcode;

#include "memory/SRAM.h"
using namespace memory;

#include "cpu6502/Assembler.h"
using namespace cpu6502;

#include "cpu6502/ProcessorStatusFlags.h"

class Cpu6502 : public ::testing::Test {
public:
    Cpu6502();

    void SetUp() override;    
    void TearDown() override;

    void helperSkipResetVector();

    Cpu6502TestBench testBench;
    SRAM sram;
};
