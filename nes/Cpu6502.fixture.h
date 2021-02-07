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

    template <class T>
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

    template <class T>
    void helperTestInternalExecutionOnMemoryData(const TestAbsolute<T>& test);
    
};

template <class T>
void Cpu6502::helperTestInternalExecutionOnMemoryData(const TestAbsolute<T>& test) {
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
        .addOpcode(std::make_unique<T>()).absolute(test.address)
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
