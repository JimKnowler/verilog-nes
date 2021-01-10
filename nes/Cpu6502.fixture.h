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

// TODO: move this to the opcode code?
const uint8_t OPCODE_NOP = 0xEA;

class Cpu6502 : public ::testing::Test {
public:
    Cpu6502();

    void SetUp() override;    
    void TearDown() override;

    void helperSkipResetVector();

    Cpu6502TestBench testBench;
    SRAM sram;
};
