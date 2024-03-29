#include "Cpu6502.fixture.hpp"

namespace {
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> kTestCasesSBCWithoutCarryIn = {
        {{0x00, 0x00}, U|N},
        {{0x00, 0x01}, U|N},
        {{0x7F, 0x00}, U|C},
        {{0x80, 0x00}, U|C|V},
        {{0xFF, 0x00}, U|C|N},
        {{0xFF, 0x01}, U|C|N},
        {{0xFF, 0x80}, U|C},
        {{0xFF, 0x7F}, U|C|V}
    };

    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> kTestCasesSBCWithCarryIn = {
        {{0x00, 0x00}, U|C|Z},
        {{0x00, 0x01}, U|N},
        {{0x7F, 0x01}, U|C},      
        {{0x80, 0x00}, U|C|N},
        {{0xFF, 0x01}, U|C|N},
        {{0xFF, 0x80}, U|C}
    };

    const std::map<uint8_t, uint8_t> kTestCasesDEC = {
        {0 + 1, U|Z},
        {(1<<7) + 1, U|N},
        {1 + 1, U}
    };
}

TEST_F(Cpu6502, ShouldImplementDEX) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x36;

    Assembler()
        .LDX().immediate(kTestData)
        .DEX()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDX
    testBench.tick(2);
    testBench.trace.clear();

    // simulate DEX and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                        .repeatEachStep(2)
        .port(o_debug_x).signal({kTestData}).repeat(6)
                        .signal({kTestData-1}).repeat(2)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementDEXProcessorStatus) {
    for (auto& testCase : kTestCasesDEC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(kTestData)
            .DEX()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementDEY) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x36;

    Assembler()
        .LDY().immediate(kTestData)
        .DEY()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDY
    testBench.tick(2);
    testBench.trace.clear();

    // simulate DEY and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({kTestData}).repeat(6)
                        .signal({kTestData-1}).repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementDEYProcessorStatus) {
    for (auto& testCase : kTestCasesDEC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDY().immediate(kTestData)
            .DEY()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCimmediate) {
    sram.clear(0);
    
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestData2 = 0x22;
    const uint8_t kExpectedData = kTestData1 - kTestData2 - 1;

    Assembler()
        .LDA().immediate(kTestData1)
        .SBC().immediate(kTestData2)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate
    testBench.tick(2);
    testBench.trace.clear();

    // simulate SBC + NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData1}).repeat(6)
                         .signal({kExpectedData}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);
        

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesSBCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData1)
            .SBC().immediate(kTestData2)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCimmediateWithCarryIn) {
    sram.clear(0);
    
    const uint8_t kTestData1 = 0x22;
    const uint8_t kTestData2 = 0x11;

    const uint8_t kExpectedData = kTestData1 - kTestData2;

    Assembler()
        .LDA().immediate(kTestData1)
        .SEC()
        .SBC().immediate(kTestData2)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate & SEC
    testBench.tick(4);
    testBench.trace.clear();

    // simulate SBC + NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({3, 4, 5, 6})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData1}).repeat(6)
                         .signal({kExpectedData}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCimmediateProcessorStatusWithCarryIn) {
    for (auto& testCase : kTestCasesSBCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData1)
            .SEC()
            .SBC().immediate(kTestData2)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementDECabsolute) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x53;

    TestAbsolute<DEC> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .expected = kTestData - 1        
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementDECabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesDEC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint16_t kTestAddress = 0x5678;
        
        Assembler assembler;
        assembler
                .CLI()
                .DEC().absolute(kTestAddress)
                .NOP()
            .org(kTestAddress)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCabsolute) {
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestData2 = 0x22;

    TestAbsolute<SBC> testAbsolute = {
        .address = 0x5678,
        .data = kTestData2,
        .port = o_debug_ac,
        .expected = kTestData1 - kTestData2 - 1,

        .preloadPort = &o_debug_ac,
        .preloadPortValue = kTestData1
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesSBCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .SBC().absolute("decrement")
                .NOP()
            .org(0x678A)
            .label("decrement")
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteWithCarryIn) {
    const uint8_t kTestData1 = 0x22;
    const uint8_t kTestData2 = 0x11;

    TestAbsolute<SBC> testAbsolute = {
        .address = 0x5678,
        .data = kTestData2,
        .port = o_debug_ac,
        .expected = kTestData1 - kTestData2,

        .presetCarry = true,

        .preloadPort = &o_debug_ac,
        .preloadPortValue = kTestData1
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteProcessorStatusWithCarryIn) {
    for (auto& testCase : kTestCasesSBCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .SEC()
                .SBC().absolute("decrement")
                .NOP()
            .org(0x4532)
            .label("decrement")
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithYWithoutCarry) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestData2 = 0x22;

    TestAbsoluteIndexed<SBC> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData2,
            .port = o_debug_ac,
            .expected = kTestData1 - kTestData2 - 1,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData1
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithYProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesSBCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        const uint8_t kY = 3;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .LDY().immediate(kY)
                .SBC().absolute("decrement").y()
                .NOP()
            .org(0x678A)
            .label("decrement")
            .org(0x678A + kY)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.trace.clear();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithYWithCarry) {
    const uint16_t kTestAddress = 0x67FE;
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestData2 = 0x22;

    TestAbsoluteIndexed<SBC> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData2,
            .port = o_debug_ac,
            .expected = kTestData1 - kTestData2 - 1,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData1
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 3,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithYProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesSBCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        const uint8_t kY = 3;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .LDY().immediate(kY)
                .SBC().absolute("decrement").y()
                .NOP()
            .org(0x67FE)
            .label("decrement")
            .org(0x67FE + kY)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.trace.clear();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithXWithoutCarry) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestData2 = 0x22;

    TestAbsoluteIndexed<SBC> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData2,
            .port = o_debug_ac,
            .expected = kTestData1 - kTestData2 - 1,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData1
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithXProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesSBCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        const uint8_t kX = 3;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .LDX().immediate(kX)
                .SBC().absolute("decrement").x()
                .NOP()
            .org(0x678A)
            .label("decrement")
            .org(0x678A + kX)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.trace.clear();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithXWithCarry) {
    const uint16_t kTestAddress = 0x67FE;
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestData2 = 0x22;

    TestAbsoluteIndexed<SBC> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData2,
            .port = o_debug_ac,
            .expected = kTestData1 - kTestData2 - 1,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData1
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 3,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithXProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesSBCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        const uint8_t kX = 3;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .LDX().immediate(kX)
                .SBC().absolute("decrement").x()
                .NOP()
            .org(0x67FE)
            .label("decrement")
            .org(0x67FE + kX)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.trace.clear();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementDECzeropage) {
    const uint8_t kTestAddress = 0x34;
    const uint8_t kTestData = 0x49;

    sram.clear(0);
    
    Assembler assembler;
    assembler
            .DEC().zp(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

    testBench.reset();
    helperSkipResetVector();

    testBench.tick(7);
    
    Trace expected = TraceBuilder()
        .port(o_address).signal({
            // INC
            0,
            1,
            0x0000 + kTestAddress,      // R
            0x0000 + kTestAddress,      // W (original value)
            0x0000 + kTestAddress,      // W (decremented value)

            // NOP
            2,
            3
        }).repeatEachStep(2)
        .port(o_rw).signal("1110011").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(7)
            .signal({kTestData}).repeat(2)
            .signal({kTestData-1}).repeat(2)
            .signal({0}).repeat(3)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(7).repeatEachStep(2);
    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementDECzeropageProcessorStatus) {
    for (auto& testCase : kTestCasesDEC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kTestAddress = 0x36;
        
        Assembler assembler;
        assembler
                .CLI()
                .DEC().zp(kTestAddress)
                .NOP()
            .org(0x0000 + kTestAddress)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementDECabsoluteIndexedWithXWithoutCarry) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData1 = 0x45;

    TestAbsoluteIndexed<DEC> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData1,
            .expected = kTestData1 - 1,
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementDECabsoluteIndexedWithXProcessorStatusWithoutCarry) {
    const uint8_t kX = 3;

    for (auto& testCase : kTestCasesDEC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDX().immediate(kX)
                .DEC().absolute("decrement").x()
                .NOP()
            .org(0x678A)
            .label("decrement")
            .org(0x678A + kX)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementDECabsoluteIndexedWithXWithCarry) {
    const uint16_t kTestAddress = 0x67FE;
    const uint8_t kTestData1 = 0x45;

    TestAbsoluteIndexed<DEC> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData1,
            .expected = kTestData1 - 1,
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 3,
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementDECabsoluteIndexedWithXProcessorStatusWithCarry) {
    const uint8_t kX = 3;

    for (auto& testCase : kTestCasesDEC) {
        const uint8_t kTestData1 = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDX().immediate(kX)
                .DEC().absolute("decrement").x()
                .NOP()
            .org(0x67FE)
            .label("decrement")
            .org(0x67FE + kX)
            .byte(kTestData1)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.trace.clear();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCzeropageIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA - kTestDataI - 1;
    
    Assembler assembler;
    assembler
        .byte(0)
        .org(kTestAddressIndexed)
            .byte(kTestDataI)
        .org(1234)
        .label("init")
            .LDA().immediate(kTestDataA)
            .LDX().immediate(kX)
        .label("start")
            .SBC().zp(kTestAddressZeroPage).x()
            .NOP()
        .org(0xfffc)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // skip LDA + LDX
    testBench.tick(4);
    testBench.trace.clear();

    // simulate SBC and NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // SBC
                            addressStart.byteIndex(),
                            addressStart.byteIndex() + 1u,
                            0x0000 + kTestAddressZeroPage,
                            kTestAddressIndexed,

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u,
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestDataA}).repeat(5).repeatEachStep(2)
                         .signal({kExpectedData}).repeatEachStep(2)
        .port(o_debug_x).signal({kX}).repeat(6).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(6).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCzeropageIndexedWithXProcessorStatus) {
    const uint8_t kTestAddressZeroPage = 0x53;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);

    for (auto& testCase : kTestCasesSBCWithoutCarryIn) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler assembler;
        assembler
            .byte(0)
            .org(kTestAddressIndexed)
                .byte(kTestDataI)
            .org(1234)
            .label("init")
                .CLI()
                .LDX().immediate(kX)
                .LDA().immediate(kTestDataA)
            .label("start")
                .SBC().zp(kTestAddressZeroPage).x()
                .NOP()
            .org(0xfffc)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCzeropageIndexedWithXWithCarryIn) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA - kTestDataI;
    
    Assembler assembler;
    assembler
        .byte(0)
        .org(kTestAddressIndexed)
            .byte(kTestDataI)
        .org(1234)
        .label("init")
            .SEC()
            .LDA().immediate(kTestDataA)
            .LDX().immediate(kX)
        .label("start")
            .SBC().zp(kTestAddressZeroPage).x()
            .NOP()
        .org(0xfffc)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // skip SEC + LDA + LDX
    testBench.tick(6);
    testBench.trace.clear();

    // simulate SBC and NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // SBC
                            addressStart.byteIndex(),
                            addressStart.byteIndex() + 1u,
                            0x0000 + kTestAddressZeroPage,
                            kTestAddressIndexed,

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u,
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestDataA}).repeat(5).repeatEachStep(2)
                         .signal({kExpectedData}).repeatEachStep(2)
        .port(o_debug_x).signal({kX}).repeat(6).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(6).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCzeropageIndexedWithXProcessorStatusWithCarryIn) {
    const uint8_t kTestAddressZeroPage = 0x53;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);

    for (auto& testCase : kTestCasesSBCWithCarryIn) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler assembler;
        assembler
            .byte(0)
            .org(kTestAddressIndexed)
                .byte(kTestDataI)
            .org(1234)
            .label("init")
                .CLI()
                .SEC()
                .LDX().immediate(kX)
                .LDA().immediate(kTestDataA)
            .label("start")
                .SBC().zp(kTestAddressZeroPage).x()
                .NOP()
            .org(0xfffc)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(14);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementDECzeropageIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);
    const uint8_t kTestDataM = 0x75;
    
    Assembler assembler;
    assembler
        .byte(0)
        .org(kTestAddressIndexed)
            .byte(kTestDataM)
        .org(1234)
        .label("init")
            .LDX().immediate(kX)
        .label("start")
            .DEC().zp(kTestAddressZeroPage).x()
            .NOP()
        .org(0xfffc)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // skip LDX
    testBench.tick(2);
    testBench.trace.clear();

    // simulate DEC and NOP
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11110011")
                    .repeatEachStep(2)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_address).signal({
                            // SBC
                            addressStart.byteIndex(),
                            addressStart.byteIndex() + 1u,
                            0x0000 + kTestAddressZeroPage,
                            kTestAddressIndexed,
                            kTestAddressIndexed,
                            kTestAddressIndexed,

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u,
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(8).repeatEachStep(2)
        .port(o_debug_x).signal({kX}).repeat(8).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(8).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    EXPECT_THAT(kTestDataM-1, sram.read(kTestAddressIndexed));
}

TEST_F(Cpu6502, ShouldImplementDECzeropageIndexedWithXProcessorStatus) {
    const uint8_t kTestAddressZeroPage = 0x53;
    const uint8_t kX = 0x7F;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);

    for (auto& testCase : kTestCasesDEC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler assembler;
        assembler
            .byte(0)
            .org(kTestAddressIndexed)
                .byte(kTestData)
            .org(1234)
            .label("init")
                .CLI()
                .LDX().immediate(kX)
            .label("start")
                .DEC().zp(kTestAddressZeroPage).x()
                .NOP()
            .org(0xfffc)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

// SBC zeropage
TEST_F(Cpu6502, ShouldImplementSBCzeropage) {
    sram.clear(0);
    
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestAddressZeroPage = 0x80;
    const uint8_t kTestData2 = 0x22;
    const uint8_t kExpectedData = kTestData1 - kTestData2 - 1;

    Assembler()
            .LDA().immediate(kTestData1)
            .SBC().zp(kTestAddressZeroPage)
            .NOP()
        .org(0x0000 + kTestAddressZeroPage)
        .byte(kTestData2)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate
    testBench.tick(2);
    testBench.trace.clear();

    // simulate SBC + NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({
                            // SBC zp
                            2, 
                            3, 
                            0x0000 + kTestAddressZeroPage, 
                            // NOP
                            4, 
                            5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData1}).repeat(8)
                         .signal({kExpectedData}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(10)
        .port(o_debug_y).signal({0x00}).repeat(10);
        

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCzeropageProcessorStatus) {
    for (auto& testCase : kTestCasesSBCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        const uint8_t kTestAddressZeroPage = 0x90;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .SBC().zp(kTestAddressZeroPage)
                .NOP()
            .org(0x0000 + kTestAddressZeroPage)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCzeropageWithCarryIn) {
    sram.clear(0);
    
    const uint8_t kTestData1 = 0x22;
    const uint8_t kTestData2 = 0x11;
    const uint8_t kTestAddressZeroPage = 0x80;

    const uint8_t kExpectedData = kTestData1 - kTestData2;

    Assembler()
            .LDA().immediate(kTestData1)
            .SEC()
            .SBC().zp(0x0000 + kTestAddressZeroPage)
            .NOP()
        .org(0x0000 + kTestAddressZeroPage)
        .byte(kTestData2)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate & SEC
    testBench.tick(4);
    testBench.trace.clear();

    // simulate SBC + NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({
                            // SBC
                            3, 
                            4,
                            0x0000 + kTestAddressZeroPage,
                            // NOP
                            5, 
                            6
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData1}).repeat(8)
                         .signal({kExpectedData}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(10)
        .port(o_debug_y).signal({0x00}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCzeropageProcessorStatusWithCarryIn) {
    for (auto& testCase : kTestCasesSBCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        const uint8_t kTestAddressZeroPage = 0x78;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .SEC()
                .SBC().zp(kTestAddressZeroPage)
                .NOP()
            .org(0x0000 + kTestAddressZeroPage)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCZeroPageIndirectIndexedWithY) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0x4;
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x33;
    const uint8_t kExpected = uint8_t(kTestPreloadA - kTestData - 1);

    Assembler assembler;
    assembler
        .NOP()
        .org(0x0000 + kTestAddressIndirect)
            .word(kTestAddress)
        .org(kTestAddress + kTestOffset)
            .byte(kTestData)
        .org(0xABCD)
        .label("init")
            .LDY().immediate(kTestOffset)
            .LDA().immediate(kTestPreloadA)
        .label("start")
            .SBC().zpIndirect(kTestAddressIndirect).y()
            .NOP()
        .org(0xFFFC)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // simulate LDY, LDA (init)
    testBench.tick(4);
    testBench.trace.clear();

    // simulate SBC (zp),y
    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // SBC (zp),y
                            addressStart.byteIndex(),
                            addressStart.byteIndex()+1u,
                            0x0000 + kTestAddressIndirect,
                            0x0001 + kTestAddressIndirect,
                            kTestAddress + kTestOffset,

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestPreloadA}).repeat(6)
                         .signal({kExpected}).repeat(1)
                        .concat()
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({kTestOffset}).repeat(7).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCZeroPageIndirectIndexedWithYWithCarry) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0xF4;
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x35;
    const uint8_t kExpected = uint8_t(kTestPreloadA - kTestData - 1);

    Assembler assembler;
    assembler
        .NOP()
        .org(0x0000 + kTestAddressIndirect)
            .word(kTestAddress)
        .org(kTestAddress + kTestOffset)
            .byte(kTestData)
        .org(0xABCD)
        .label("init")
            .LDY().immediate(kTestOffset)
            .LDA().immediate(kTestPreloadA)
        .label("start")
            .SBC().zpIndirect(kTestAddressIndirect).y()
            .NOP()
        .org(0xFFFC)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // simulate LDY, LDA (init)
    testBench.tick(4);
    testBench.trace.clear();

    // simulate SBC (zp),y
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // SBC (zp),y
                            addressStart.byteIndex(),
                            addressStart.byteIndex()+1u,
                            0x0000 + kTestAddressIndirect,
                            0x0001 + kTestAddressIndirect,
                            (kTestAddress & 0xff00) + ((kTestAddress + kTestOffset) & 0x00ff),  // without carry
                            kTestAddress + kTestOffset,                                         // with carry

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestPreloadA}).repeat(7)
                         .signal({kExpected}).repeat(1)
                        .concat()
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(8).repeatEachStep(2)
        .port(o_debug_y).signal({kTestOffset}).repeat(8).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCZeroPageIndirectIndexedWithYProcessorStatus) {
    for (auto& testCase : kTestCasesSBCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kTestAddressIndirect = 42;
        const uint16_t kTestAddress = 0x1230;
        const uint8_t kTestOffset = 0x04;

        Assembler assembler;
        assembler
            .NOP()
            .org(0x0000 + kTestAddressIndirect)
                .word(kTestAddress)
            .org(kTestAddress + kTestOffset)
                .byte(kTestData2)
            .org(0xABCD)
            .label("init")
                .CLI()
                .LDY().immediate(kTestOffset)
                .LDA().immediate(kTestData1)
            .label("start")
                .SBC().zpIndirect(kTestAddressIndirect).y()
                .NOP()
            .org(0xFFFC)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCZeroPageIndirectIndexedWithYProcessorStatusWithCarryIn) {
    for (auto& testCase : kTestCasesSBCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kTestAddressIndirect = 42;
        const uint16_t kTestAddress = 0x1230;
        const uint8_t kTestOffset = 0x04;

        Assembler assembler;
        assembler
            .NOP()
            .org(0x0000 + kTestAddressIndirect)
                .word(kTestAddress)
            .org(kTestAddress + kTestOffset)
                .byte(kTestData2)
            .org(0xABCD)
            .label("init")
                .CLI()
                .SEC()
                .LDY().immediate(kTestOffset)
                .LDA().immediate(kTestData1)
            .label("start")
                .SBC().zpIndirect(kTestAddressIndirect).y()
                .NOP()
            .org(0xFFFC)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(15);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCZeroPageIndirectIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kTestOffset = 0x40;
    const uint16_t kTestAddressZeroPageWithOffset = 0x0000 + uint8_t(kTestAddressZeroPage + kTestOffset);
    const uint16_t kTestAddress = 0x1230;    
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x33;
    const uint8_t kExpected = uint8_t(kTestPreloadA - kTestData - 1);

    Assembler assembler;
    assembler
        .NOP()
        .org(kTestAddressZeroPageWithOffset)
            .word(kTestAddress)
        .org(kTestAddress)
            .byte(kTestData)
        .org(0xABCD)
        .label("init")
            .LDX().immediate(kTestOffset)
            .LDA().immediate(kTestPreloadA)
        .label("start")
            .SBC().zpIndirect(kTestAddressZeroPage).x()
            .NOP()
        .org(0xFFFC)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // simulate LDY, LDA (init)
    testBench.tick(4);
    testBench.trace.clear();

    // simulate SBC (zp,x)
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // SBC (zp,x)
                            addressStart.byteIndex(),
                            addressStart.byteIndex() + 1u,
                            0x0000 + kTestAddressZeroPage,
                            kTestAddressZeroPageWithOffset,
                            kTestAddressZeroPageWithOffset + 1u,
                            kTestAddress,

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestPreloadA}).repeat(7)
                         .signal({kExpected}).repeat(1)
                        .concat()
                        .repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(8).repeatEachStep(2)
        .port(o_debug_x).signal({kTestOffset}).repeat(8).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCZeroPageIndirectIndexedWithXProcessorStatus) {
    for (auto& testCase : kTestCasesSBCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kTestAddressZeroPage = 0x42;
        const uint8_t kTestOffset = 0x40;
        const uint16_t kTestAddressZeroPageWithOffset = 0x0000 + uint8_t(kTestAddressZeroPage + kTestOffset);
        const uint16_t kTestAddress = 0x1230;    

        Assembler assembler;
        assembler
                .NOP()
            .org(kTestAddressZeroPageWithOffset)
                .word(kTestAddress)
            .org(kTestAddress)
                .byte(kTestData2)            
            .org(0xABCD)
            .label("init")
                .CLI()
                .LDX().immediate(kTestOffset)
                .LDA().immediate(kTestData1)
            .label("start")
                .SBC().zpIndirect(kTestAddressZeroPage).x()
                .NOP()
            .org(0xFFFC)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(14);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCZeroPageIndirectIndexedWithXProcessorStatusWithCarryIn) {
    for (auto& testCase : kTestCasesSBCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kTestAddressZeroPage = 0x42;
        const uint8_t kTestOffset = 0x40;
        const uint16_t kTestAddressZeroPageWithOffset = 0x0000 + uint8_t(kTestAddressZeroPage + kTestOffset);
        const uint16_t kTestAddress = 0x1230;    

        Assembler assembler;
        assembler
                .NOP()
            .org(kTestAddressZeroPageWithOffset)
                .word(kTestAddress)
            .org(kTestAddress)
                .byte(kTestData2)            
            .org(0xABCD)
            .label("init")
                .CLI()
                .SEC()
                .LDX().immediate(kTestOffset)
                .LDA().immediate(kTestData1)
            .label("start")
                .SBC().zpIndirect(kTestAddressZeroPage).x()
                .NOP()
            .org(0xFFFC)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(16);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

// TODO: SBC a,x + SBC a,y with 'carry in'