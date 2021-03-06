// note: included inline from Cpu6502.fixture.hpp

template <class OPCODE>
void Cpu6502::helperTestInternalExecutionOnMemoryData(const TestAbsolute<OPCODE>& test) {
    sram.clear(0);
    
    Assembler assembler;

    if (test.preloadPort == &o_debug_ac) {
        assembler.LDA().immediate(test.preloadPortValue);
    } else if (test.preloadPort == &o_debug_x) {
        assembler.LDX().immediate(test.preloadPortValue);
    } else if (test.preloadPort == &o_debug_y) {
        assembler.LDY().immediate(test.preloadPortValue);
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
        const uint8_t preloadPortValue = (port == test.preloadPort) ? test.preloadPortValue : 0xFF;

        if (port->id() == test.port.id()) {
            traceBuilder
                .port(*port)
                    .signal({preloadPortValue}).repeat(5)
                    .signal({test.expected}).repeat(1)
                    .concat().repeatEachStep(2);
        } else {
            traceBuilder
                .port(*port)
                    .signal({preloadPortValue}).repeat(12);  
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

template <class OPCODE>
void Cpu6502::helperTestInternalExecutionOnMemoryData(const TestAbsoluteIndexed<OPCODE>& test) {
    sram.clear(0);
    
    Assembler assembler;
    uint16_t pc = 0;
    int skipTicks = 0;

    if (test.preloadPort != nullptr) {
        pc += 2;
        skipTicks += 2;
        
        if (test.preloadPort == &o_debug_ac) {
            assembler.LDA().immediate(test.preloadPortValue);
        } else if (test.preloadPort == &o_debug_x) {
            assembler.LDX().immediate(test.preloadPortValue);
        } else if (test.preloadPort == &o_debug_y) {
            assembler.LDY().immediate(test.preloadPortValue);
        }
    }

    switch (test.indexRegister) {
        case kX:
            assembler.LDX().immediate(test.preloadIndexRegisterValue);
            break;
        case kY:
            assembler.LDY().immediate(test.preloadIndexRegisterValue);
            break;
        default:
            assert(!"unknown index register");
    }
    pc += 2;
    skipTicks += 2;

    if (test.presetCarry) {
        assembler.SEC();
        pc += 1;
        skipTicks +=2 ;
    }

    assembler
        .addOpcode(std::make_unique<OPCODE>()).absolute(test.address);
    
    switch (test.indexRegister) {
        case kX:
            assembler.x();
            break;
        case kY:
            assembler.y();
            break;
        default:
            assert(!"unknown index register");
    }

    // determine whether 6502 ALU will carry at T3, requiring extra T4
    bool hasCarryTick = ((test.address + test.preloadIndexRegisterValue) & 0xFF)
                        < (test.address & 0xFF);

    assembler
        .NOP()
        .org(test.address + test.preloadIndexRegisterValue)
        .word(test.data)
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(skipTicks);
    testBench.trace.clear();

    testBench.tick(6);
    if (hasCarryTick) {
        // an extra clock cycle when address + index carries, so ALU
        // requires extra T4 to implement addition on high byte
        testBench.tick(1);
    }

    struct PreloadPort {
        PortDescription* port;
        uint8_t value;
    };

    auto getPreloadValue = [&](PortDescription* port) -> uint8_t {
        uint8_t preloadValue = (port == test.preloadPort) ? test.preloadPortValue : 0xFF;

        if ( ((port == &o_debug_x) && (test.indexRegister == kX))
            ||  ((port == &o_debug_y) && (test.indexRegister == kY)) ) {
            preloadValue = test.preloadIndexRegisterValue;
        }

        return preloadValue;
    };

    const std::vector<PreloadPort> kPreloadPorts = { 
        { &o_debug_ac, getPreloadValue(&o_debug_ac) },
        { &o_debug_x, getPreloadValue(&o_debug_x) },
        { &o_debug_y, getPreloadValue(&o_debug_y) }
    };

    TraceBuilder traceBuilderOpcode;
    traceBuilderOpcode
        .port(i_clk).signal("_-").repeat(4)
        .port(o_rw).signal("11").repeat(4)
        .port(o_sync).signal("1000").repeatEachStep(2)
        .port(o_address)
            .signal({
                pc + 0u,
                pc + 1u,
                pc + 2u,
                (test.address & 0xff00u) + ((test.address + test.preloadIndexRegisterValue) & 0x00ffu),
            })
            .repeatEachStep(2);

    for (auto& preloadPort: kPreloadPorts) {
        traceBuilderOpcode
            .port(*preloadPort.port).signal({preloadPort.value}).repeat(8); 
    }
    
    Trace expectedOpcode = traceBuilderOpcode;

    Trace expectedCarry;
    if (hasCarryTick) {
        TraceBuilder traceBuilderCarry;
        traceBuilderCarry
            .port(i_clk).signal("_-")
            .port(o_rw).signal("11")
            .port(o_sync).signal("0").repeatEachStep(2)
            .port(o_address)
                .signal({
                    static_cast<uint32_t>(test.address + test.preloadIndexRegisterValue)
                })
                .repeatEachStep(2);
        
        for (auto& preloadPort: kPreloadPorts) {
            traceBuilderCarry
                .port(*preloadPort.port).signal({preloadPort.value}).repeat(2); 
        }
        
        expectedCarry = traceBuilderCarry;
    }
    
    TraceBuilder traceBuilderNop;
    traceBuilderNop
        .port(i_clk).signal("_-").repeat(2)
        .port(o_rw).signal("11").repeat(2)
        .port(o_sync).signal("10").repeatEachStep(2)
        .port(o_address)
            .signal({
                pc + 3u,
                pc + 4u
            })
            .repeatEachStep(2);

    for (auto& preloadPort: kPreloadPorts) {
        if (preloadPort.port->id() == test.port.id()) {
            traceBuilderNop
                .port(*preloadPort.port)
                    .signal({preloadPort.value}).repeat(2)
                    .signal({test.expected}).repeat(2);
        } else {
            traceBuilderNop
                .port(*preloadPort.port)
                    .signal({preloadPort.value}).repeat(4);  
        }
    }

    Trace expectedNop = traceBuilderNop;

    Trace expected = expectedOpcode + expectedCarry + expectedNop;

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

template <class OPCODE>
void Cpu6502::helperTestStore(const TestAbsoluteIndexed<OPCODE>& test) {
    sram.clear(0);
    
    Assembler assembler;
    uint16_t pc = 0;
    int skipTicks = 0;

    if (test.preloadPort != nullptr) {
        pc += 2;
        skipTicks += 2;
        
        if (test.preloadPort == &o_debug_ac) {
            assembler.LDA().immediate(test.preloadPortValue);
        } else if (test.preloadPort == &o_debug_x) {
            assembler.LDX().immediate(test.preloadPortValue);
        } else if (test.preloadPort == &o_debug_y) {
            assembler.LDY().immediate(test.preloadPortValue);
        }
    }

    switch (test.indexRegister) {
        case kX:
            assembler.LDX().immediate(test.preloadIndexRegisterValue);
            break;
        case kY:
            assembler.LDY().immediate(test.preloadIndexRegisterValue);
            break;
        default:
            assert(!"unknown index register");
    }
    pc += 2;
    skipTicks += 2;

    assembler
        .addOpcode(std::make_unique<OPCODE>()).absolute(test.address);
    
    switch (test.indexRegister) {
        case kX:
            assembler.x();
            break;
        case kY:
            assembler.y();
            break;
        default:
            assert(!"unknown index register");
    }

    assembler
        .NOP()
        .org(test.address + test.preloadIndexRegisterValue)
        .word(test.data)
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(skipTicks);
    testBench.trace.clear();

    testBench.tick(7);

    const uint16_t kStoreAddress = test.address + test.preloadIndexRegisterValue;

    TraceBuilder traceBuilder;
    traceBuilder
        .port(i_clk).signal("_-").repeat(7)
        .port(o_rw).signal("11").repeat(7)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_address)
            .signal({
                // store
                pc + 0u,
                pc + 1u,
                pc + 2u,
                kStoreAddress,
                kStoreAddress,

                // NOP
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
        const uint8_t preloadPortValue = (port == test.preloadPort) ? test.preloadPortValue : 0xFF;

        traceBuilder
            .port(*port)
                .signal({preloadPortValue}).repeat(7);    
    }

    Trace expected = traceBuilder;

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
    EXPECT_EQ(test.data, sram.read(test.address));
}