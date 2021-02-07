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
        uint8_t expected;
        uint8_t expectedTick = 4;
        
        bool presetCarry = false;

        PortDescription* preloadPort = nullptr;
        uint8_t preloadValue;
    };

    template <class OPCODE>
    void helperTestInternalExecutionOnMemoryData(const TestAbsolute<OPCODE>& test);
    
    template <class OPCODE>
    void helperTestReadModifyWrite(const TestAbsolute<OPCODE>& test);
};

template <class OPCODE>
void Cpu6502::helperTestInternalExecutionOnMemoryData(const TestAbsolute<OPCODE>& test) {
    sram.clear(0);
    
    Assembler assembler;

    if (test.preloadPort == &o_debug_ac) {
        assembler.LDA().immediate(test.preloadValue);
    } else if (test.preloadPort == &o_debug_x) {
        assembler.LDX().immediate(test.preloadValue);
    } else if (test.preloadPort == &o_debug_y) {
        assembler.LDY().immediate(test.preloadValue);
    }

    if (test.presetCarry) {
        assembler.SEC();
    }

    assembler
        .addOpcode(std::make_unique<OPCODE>()).absolute(test.address)
        .NOP()
        .compileTo(sram);

    sram.write(test.address, test.data);

    helperSkipResetVector();

    uint16_t pc = 0;

    if (test.preloadPort != nullptr) {
        testBench.tick(2);
    
        pc += 2;
    }

    if (test.presetCarry) {
        testBench.tick(2);
    
        pc += 1;
    }

    testBench.trace.clear();
    testBench.tick(6);

    TraceBuilder traceBuilder;
    traceBuilder
        .port(i_clk).signal("_-").repeat(6)
        .port(o_rw).signal("11").repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address)
            .signal({
                pc + 0u,
                pc + 1u,
                pc + 2u,
                test.address,
                pc + 3u,
                pc + 4u
            })
            .repeatEachStep(2);

    std::vector<PortDescription*> ports = { 
        &o_debug_ac,
        &o_debug_x,
        &o_debug_y
    };

    for (auto& port: ports) {
        const uint8_t preloadValue = (port == test.preloadPort) ? test.preloadValue : 0xFF;

        if (port->id() == test.port.id()) {
            traceBuilder
                .port(*port)
                    .signal({preloadValue}).repeat(test.expectedTick)
                    .signal({test.expected}).repeat(6 - test.expectedTick)
                    .concat().repeatEachStep(2);
        } else {
            traceBuilder
                .port(*port)
                    .signal({preloadValue}).repeat(12);  
        }
    }

    Trace expected = traceBuilder;

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

template <class OPCODE>
void Cpu6502::helperTestReadModifyWrite(const TestAbsolute<OPCODE>& test) {
    sram.clear(0);

    Assembler assembler;

    if (test.presetCarry) {
        assembler.SEC();
    }

    assembler
            .addOpcode(std::make_unique<OPCODE>()).absolute(test.address)
            .NOP()
        .org(test.address)
        .byte(test.data)
        .compileTo(sram);

    helperSkipResetVector();

    uint16_t pc = 0;

    if (test.presetCarry) {
        testBench.tick(2);
    
        pc += 1;
    }

    testBench.trace.clear();
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11110011")
                    .repeatEachStep(2)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_data).signal({0}).repeat(9)
                     .signal({test.data}).repeat(2)
                     .signal({test.expected}).repeat(2)
                     .signal({0}).repeat(3)
        .port(o_address).signal({
                            // <OPCODE>
                            pc,
                            pc + 1u,
                            pc + 2u,
                            test.address,
                            test.address,
                            test.address,
                            // NOP
                            pc + 3u,
                            pc + 4u
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF})
                        .repeat(16)
        .port(o_debug_x).signal({0xFF})
                        .repeat(16)
        .port(o_debug_y).signal({0xFF})
                        .repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    EXPECT_EQ(test.expected, sram.read(test.address));
}