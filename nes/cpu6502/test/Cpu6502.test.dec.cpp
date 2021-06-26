#include "Cpu6502.fixture.hpp"

namespace {
    // todo: compare this to real 6502
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> kTestCasesSBCWithCarryIn = {
        {{0x00, 0x00}, N},
        {{0x00, 0x01}, N},
        {{0x7F, 0x00}, C},
        {{0x80, 0x00}, C|V},
        {{0xFF, 0x00}, C|N},
        {{0xFF, 0x01}, C|N},
        {{0xFF, 0x80}, C},
        {{0xFF, 0x7F}, C|V}
    };

    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> kTestCasesSBC = {
        {{0x00, 0x00}, C|Z},
        {{0x00, 0x01}, N},
        {{0x7F, 0x01}, C},      
        {{0x80, 0x00}, C|N},
        {{0xFF, 0x01}, C|N},
        {{0xFF, 0x80}, C}
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
        .port(o_debug_ac).signal({0xFF, 0xFF})
                        .repeatEachStep(4)
        .port(o_debug_x).signal({kTestData}).repeat(6)
                        .signal({kTestData-1}).repeat(2)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementDEXProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0 + 1, Z},
        {(1<<7) + 1, N},
        {1 + 1, 0}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDX().immediate(kTestData)
            .DEX()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
        .port(o_debug_ac).signal({0xFF, 0xFF})
                        .repeatEachStep(4)
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({kTestData}).repeat(6)
                        .signal({kTestData-1}).repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementDEYProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0 + 1, Z},
        {(1<<7) + 1, N},
        {1 + 1, 0}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDY().immediate(kTestData)
            .DEY()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCimmediate) {
    sram.clear(0);
    
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestData2 = 0x22;
    const uint8_t kExpectedData = kTestData1 - kTestData2;

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
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);
        

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesSBC) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDA().immediate(kTestData1)
            .SBC().immediate(kTestData2)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCimmediateWithCarryIn) {
    sram.clear(0);
    
    const uint8_t kTestData1 = 0x22;
    const uint8_t kTestData2 = 0x11;

    // todo: compare this to real 6502
    const uint8_t kExpectedData = kTestData1 - kTestData2 - 1;

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
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSBCimmediateProcessorStatusWithCarryIn) {
    for (auto& testCase : kTestCasesSBCWithCarryIn) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDA().immediate(kTestData1)
            .SEC()
            .SBC().immediate(kTestData2)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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
    const std::map<uint8_t, uint8_t> testCases = {
        {0 + 1, Z},
        {(1<<7) + 1, N},
        {1 + 1, 0}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint16_t kTestAddress = 0x5678;
        
        Assembler assembler;
        assembler
                .DEC().absolute(kTestAddress)
                .NOP()
            .org(kTestAddress)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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
        .expected = kTestData1 - kTestData2,

        .preloadPort = &o_debug_ac,
        .preloadPortValue = kTestData1
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesSBC) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .LDA().immediate(kTestData1)
                .SBC().absolute("decrement")
                .NOP()
            .org(0x678A)
            .label("decrement")
            .byte(kTestData2)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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
        .expected = kTestData1 - kTestData2 - 1,

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

        testBench.tick(10);
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
            .expected = kTestData1 - kTestData2,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData1
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithYProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesSBC) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        const uint8_t kY = 3;

        sram.clear(0);
    
        Assembler()
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

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithYWithCarry) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData1 = 0x45;
    const uint8_t kTestData2 = 0x22;

    TestAbsoluteIndexed<SBC> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData2,
            .port = o_debug_ac,
            .expected = kTestData1 - kTestData2,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData1
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 3,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSBCabsoluteIndexedWithYProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesSBC) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        const uint8_t kY = 3;

        sram.clear(0);
    
        Assembler()
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

        testBench.tick(11);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}