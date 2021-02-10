#pragma once

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/Cpu6502TestBench.h"
using namespace cpu6502testbench;

#include "memory/SRAM.h"
using namespace memory;

#include "cpu6502/assembler/Assembler.h"
using namespace cpu6502::assembler;

#include "cpu6502/ProcessorStatusFlags.h"

class Cpu6502 : public ::testing::Test {
public:
    Cpu6502();

    void SetUp() override;    
    void TearDown() override;

    void helperSkipResetVector();

    Cpu6502TestBench testBench;
    SRAM sram;

    template <class OPCODE>
    struct TestAbsolute {
        uint16_t address;
        uint8_t data;
        PortDescription port;
        uint8_t expected;                   // expected value in port at end of test
        
        int expectedTick = 4;               // tick when expected data should arrive
                                            // in the port
                                            // TODO: remove this when gtestverilog supports wildcards
                                            //       or if verilog is standardised to 5 ticks..
        
        bool presetCarry = false;

        PortDescription* preloadPort = nullptr;
        uint8_t preloadPortValue;
    };

    enum IndexRegister {
        kX,
        kY
    };

    template <class OPCODE>
    struct TestAbsoluteIndexed : public TestAbsolute<OPCODE> {
        IndexRegister   indexRegister;
        uint8_t         preloadIndexRegisterValue;
    };

    template <class OPCODE>
    void helperTestInternalExecutionOnMemoryData(const TestAbsolute<OPCODE>& test);
    
    template <class OPCODE>
    void helperTestReadModifyWrite(const TestAbsolute<OPCODE>& test);

    template <class OPCODE>
    void helperTestInternalExecutionOnMemoryData(const TestAbsoluteIndexed<OPCODE>& test);
};

// inlined template implementations
#include "Cpu6502.fixture.inl"
