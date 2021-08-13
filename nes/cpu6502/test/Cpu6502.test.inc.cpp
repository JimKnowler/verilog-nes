#include "Cpu6502.fixture.hpp"

namespace {
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> kTestCasesADCWithCarryIn = {
        {{0x00, 0x00}, U},
        {{0x00, 0x01}, U},
        {{0x7F, 0x00}, U|N|V},
        {{0x80, 0x00}, U|N},
        {{0xFF, 0x00}, U|C|Z},
        {{0xFF, 0x01}, U|C},
        {{0xFF, 0x80}, U|C|N},
        {{0xFF, 0x7F}, U|C}
    };

    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> kTestCasesADCWithoutCarryIn = {
        {{0x00, 0x00}, U|Z},
        {{0x00, 0x01}, U},
        {{0x7F, 0x01}, U|N|V},
        {{0x80, 0x00}, U|N},
        {{0xFF, 0x01}, U|C|Z},
        {{0xFF, 0x80}, U|C|V}
    };

    const std::map<uint8_t, uint8_t> kTestCasesINC = {
        {0 - 1, U|Z},
        {(1<<7) - 1, U|N},
        {1 - 1, U}
    };
}

TEST_F(Cpu6502, ShouldImplementINX) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x93;

    Assembler()
        .LDX().immediate(kTestData)
        .INX()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDXimmediate
    testBench.tick(2);
    testBench.trace.clear();

    // simulate INX and NOP
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
                        .signal({kTestData+1}).repeat(2)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementINXProcessorStatus) {
    for (auto& testCase : kTestCasesINC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(kTestData)
            .INX()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementINY) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x45;

    Assembler()
        .LDY().immediate(kTestData)
        .INY()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDYimmediate
    testBench.tick(2);
    testBench.trace.clear();

    // simulte INY + NOP
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
                        .signal({kTestData+1}).repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementINYProcessorStatus) {
    for (auto& testCase : kTestCasesINC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDY().immediate(kTestData)
            .INY()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementADCimmediate) {
    sram.clear(0);
    
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestData2 = 0x22;

    Assembler()
        .LDA().immediate(kTestData1)
        .ADC().immediate(kTestData2)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate
    testBench.tick(2);
    testBench.trace.clear();

    // simulate ADC + NOP
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
                         .signal({kTestData1+kTestData2}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);
        

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementADCimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesADCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData1)
            .ADC().immediate(kTestData2)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementADCimmediateWithCarryIn) {
    sram.clear(0);
    
    const uint8_t kTestData1 = 0x22;
    const uint8_t kTestData2 = 0x31;

    Assembler()
        .LDA().immediate(kTestData1)
        .SEC()
        .ADC().immediate(kTestData2)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate & SEC
    testBench.tick(4);
    testBench.trace.clear();

    // simulate ADC + NOP
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
                         .signal({kTestData1+kTestData2+1}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementADCimmediateProcessorStatusWithCarryIn) {
    for (auto& testCase : kTestCasesADCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData1)
            .SEC()
            .ADC().immediate(kTestData2)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementINCabsolute) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x49;

    TestAbsolute<INC> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .expected = kTestData + 1        
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementINCabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesINC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint16_t kTestAddress = 0x5678;
        
        Assembler assembler;
        assembler
                .CLI()
                .INC().absolute(kTestAddress)
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

TEST_F(Cpu6502, ShouldImplementADCabsolute) {
    const uint8_t kTestData1 = 0x42;
    const uint8_t kTestData2 = 0x22;

    TestAbsolute<ADC> testAbsolute = {
        .address = 0x5678,
        .data = kTestData1,
        .port = o_debug_ac,
        .expected = kTestData1 + kTestData2,

        .preloadPort = &o_debug_ac,
        .preloadPortValue = kTestData2
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesADCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .ADC().absolute("increment")
                .NOP()
            .org(0x678A)
            .label("increment")
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteWithCarryIn) {
    const uint8_t kTestData1 = 0x22;
    const uint8_t kTestData2 = 0x31;

    TestAbsolute<ADC> testAbsolute = {
        .address = 0x5678,
        .data = kTestData1,
        .port = o_debug_ac,
        .expected = kTestData1 + kTestData2 + 1,

        .presetCarry = true,

        .preloadPort = &o_debug_ac,
        .preloadPortValue = kTestData2
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteProcessorStatusWithCarryIn) {
    for (auto& testCase : kTestCasesADCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .SEC()
                .ADC().absolute("increment")
                .NOP()
            .org(0x4532)
            .label("increment")
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementINCzeropage) {
    const uint8_t kTestAddress = 0x78;
    const uint8_t kTestData = 0x49;

    sram.clear(0);
    
    Assembler assembler;
    assembler
            .INC().zp(kTestAddress)
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
            0x0000 + kTestAddress,      // W (incremented value)

            // NOP
            2,
            3
        }).repeatEachStep(2)
        .port(o_rw).signal("1110011").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(7)
            .signal({kTestData}).repeat(2)
            .signal({kTestData+1}).repeat(2)
            .signal({0}).repeat(3)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(7).repeatEachStep(2);
    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementINCzeropageProcessorStatus) {
    for (auto& testCase : kTestCasesINC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kTestAddress = 0x56;
        
        Assembler assembler;
        assembler
                .CLI()
                .INC().zp(kTestAddress)
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

TEST_F(Cpu6502, ShouldImplementADCzeropage) {
    sram.clear(0);
    
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestData2 = 0x22;
    const uint8_t kTestAddress = 0x89;

    Assembler()
            .LDA().immediate(kTestData1)
            .ADC().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(kTestData2)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate
    testBench.tick(2);
    testBench.trace.clear();

    // simulate ADC + NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({2, 3, kTestAddress, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData1}).repeat(8)
                         .signal({kTestData1+kTestData2}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(10)
        .port(o_debug_y).signal({0x00}).repeat(10);
        

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementADCzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0x78;

    for (auto& testCase : kTestCasesADCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .ADC().zp(kTestAddress)
                .NOP()
            .org(kTestAddress)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementADCzeropageWithCarryIn) {
    sram.clear(0);
    
    const uint8_t kTestData1 = 0x22;
    const uint8_t kTestData2 = 0x31;
    const uint8_t kTestAddress = 0x94;

    Assembler()
            .LDA().immediate(kTestData1)
            .SEC()
            .ADC().zp(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData2)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate & SEC
    testBench.tick(4);
    testBench.trace.clear();

    // simulate ADC + NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({3, 4, kTestAddress, 5, 6})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData1}).repeat(8)
                         .signal({kTestData1+kTestData2+1}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(10)
        .port(o_debug_y).signal({0x00}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementADCzeropageProcessorStatusWithCarryIn) {
    const uint8_t kTestAddress = 83;

    for (auto& testCase : kTestCasesADCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .SEC()
                .ADC().zp(kTestAddress)
                .NOP()
            .org(kTestAddress)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(11);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteIndexedWithX) {
    const uint8_t kTestData1 = 0x42;
    const uint8_t kTestData2 = 0x22;

    TestAbsoluteIndexed<ADC> testAbsolute = {
        {
            .address = 0x5678,
            .data = kTestData1,
            .port = o_debug_ac,
            .expected = kTestData1 + kTestData2,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData2
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteIndexedWithXProcessorStatus) {
    const uint8_t kX = 5;

    for (auto& testCase : kTestCasesADCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDX().immediate(kX)
                .LDA().immediate(kTestData1)
                .ADC().absolute("increment").x()
                .NOP()
            .org(0x678A)
            .label("increment")
            .org(0x678A + kX)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteIndexedWithXWithCarryIn) {
    const uint8_t kTestData1 = 0x22;
    const uint8_t kTestData2 = 0x31;

    TestAbsoluteIndexed<ADC> testAbsolute = {
        {
            .address = 0x5678,
            .data = kTestData1,
            .port = o_debug_ac,
            .expected = kTestData1 + kTestData2 + 1,

            .presetCarry = true,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData2
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 0x5
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteIndexedWithXProcessorStatusWithCarryIn) {
    const uint8_t kX = 8;

    for (auto& testCase : kTestCasesADCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .LDX().immediate(kX)
                .SEC()
                .ADC().absolute("increment").x()
                .NOP()
            .org(0x4532)
            .label("increment")
            .org(0x4532+kX)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(15);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteIndexedWithY) {
    const uint8_t kTestData1 = 0x42;
    const uint8_t kTestData2 = 0x22;

    TestAbsoluteIndexed<ADC> testAbsolute = {
        {
            .address = 0x5678,
            .data = kTestData1,
            .port = o_debug_ac,
            .expected = kTestData1 + kTestData2,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData2
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteIndexedWithYProcessorStatus) {
    const uint8_t kY = 5;

    for (auto& testCase : kTestCasesADCWithoutCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDY().immediate(kY)
                .LDA().immediate(kTestData1)
                .ADC().absolute("increment").y()
                .NOP()
            .org(0x678A)
            .label("increment")
            .org(0x678A + kY)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteIndexedWithYWithCarryIn) {
    const uint8_t kTestData1 = 0x22;
    const uint8_t kTestData2 = 0x31;

    TestAbsoluteIndexed<ADC> testAbsolute = {
        {
            .address = 0x5678,
            .data = kTestData1,
            .port = o_debug_ac,
            .expected = kTestData1 + kTestData2 + 1,

            .presetCarry = true,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData2
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 0x5
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteIndexedWithYProcessorStatusWithCarryIn) {
    const uint8_t kY = 8;

    for (auto& testCase : kTestCasesADCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestData1)
                .LDY().immediate(kY)
                .SEC()
                .ADC().absolute("increment").y()
                .NOP()
            .org(0x4532)
            .label("increment")
            .org(0x4532+kY)
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(15);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteIndexedWithXWithCarry) {
    const uint8_t kTestData1 = 0x42;
    const uint8_t kTestData2 = 0x22;

    TestAbsoluteIndexed<ADC> testAbsolute = {
        {
            .address = 0x56FE,
            .data = kTestData1,
            .port = o_debug_ac,
            .expected = kTestData1 + kTestData2,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData2
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementADCabsoluteIndexedWithYWithCarry) {
    const uint8_t kTestData1 = 0x42;
    const uint8_t kTestData2 = 0x22;

    TestAbsoluteIndexed<ADC> testAbsolute = {
        {
            .address = 0x56FE,
            .data = kTestData1,
            .port = o_debug_ac,
            .expected = kTestData1 + kTestData2,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData2
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementINCabsoluteIndexedWithXWithoutCarry) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData1 = 0x45;

    TestAbsoluteIndexed<INC> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData1,
            .expected = kTestData1 + 1,
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementINCabsoluteIndexedWithXProcessorStatusWithoutCarry) {
    const uint8_t kX = 3;

    for (auto& testCase : kTestCasesINC) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDX().immediate(kX)
                .INC().absolute("increment").x()
                .NOP()
            .org(0x678A)
            .label("increment")
            .org(0x678A + kX)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementINCabsoluteIndexedWithXWithCarry) {
    const uint16_t kTestAddress = 0x67FE;
    const uint8_t kTestData1 = 0x45;

    TestAbsoluteIndexed<INC> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData1,
            .expected = kTestData1 + 1,
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 3,
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementINCabsoluteIndexedWithXProcessorStatusWithCarry) {
    const uint8_t kX = 3;

    for (auto& testCase : kTestCasesINC) {
        const uint8_t kTestData1 = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDX().immediate(kX)
                .INC().absolute("increment").x()
                .NOP()
            .org(0x67FE)
            .label("increment")
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

TEST_F(Cpu6502, ShouldImplementADCzeropageIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA + kTestDataI;
    
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
            .ADC().zp(kTestAddressZeroPage).x()
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

    // simulate ADC and NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // ADC
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

TEST_F(Cpu6502, ShouldImplementADCzeropageIndexedWithXProcessorStatus) {
    const uint8_t kTestAddressZeroPage = 0x53;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);

    for (auto& testCase : kTestCasesADCWithoutCarryIn) {
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
                .ADC().zp(kTestAddressZeroPage).x()
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

TEST_F(Cpu6502, ShouldImplementADCzeropageIndexedWithXWithCarryIn) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA + kTestDataI + 1;
    
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
            .ADC().zp(kTestAddressZeroPage).x()
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

    // simulate ADC and NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // ADC
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

TEST_F(Cpu6502, ShouldImplementADCzeropageIndexedWithXProcessorStatusWithCarryIn) {
    const uint8_t kTestAddressZeroPage = 0x53;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);

    for (auto& testCase : kTestCasesADCWithCarryIn) {
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
                .ADC().zp(kTestAddressZeroPage).x()
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

TEST_F(Cpu6502, ShouldImplementINCzeropageIndexedWithX) {
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
            .INC().zp(kTestAddressZeroPage).x()
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

    // simulate INC and NOP
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11110011")
                    .repeatEachStep(2)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_address).signal({
                            // INC
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

    EXPECT_THAT(kTestDataM+1, sram.read(kTestAddressIndexed));
}

TEST_F(Cpu6502, ShouldImplementINCzeropageIndexedWithXProcessorStatus) {
    const uint8_t kTestAddressZeroPage = 0x53;
    const uint8_t kX = 0x7F;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);

    for (auto& testCase : kTestCasesINC) {
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
                .INC().zp(kTestAddressZeroPage).x()
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