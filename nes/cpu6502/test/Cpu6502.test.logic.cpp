#include "Cpu6502.fixture.hpp"

namespace {
    const std::map<std::pair<uint8_t,uint8_t>, uint8_t> kTestCasesORA = {
        {{0, 0}, U|Z},
        {{0xff, 0xff}, U|N},
        {{0x80, 0x40}, U|N},
        {{0x0, 0x80}, U|N},
        {{0x01, 0x02}, U},
        {{0x00, 0x01}, U}
    };

    const std::map<std::pair<uint8_t,uint8_t>, uint8_t> kTestCasesAND = {
        {{1, 0}, U|Z},
        {{0x80, 0x80}, U|N},
        {{1, 1}, U}
    };

    const std::map<std::pair<uint8_t,uint8_t>, uint8_t> kTestCasesEOR = {
        {{0, 0}, U|Z},
        {{0xff, 0xff}, U|Z},
        {{0x80, 0x40}, U|N},
        {{0x0, 0x80}, U|N},
        {{0x01, 0x02}, U}
    };
}

TEST_F(Cpu6502, ShouldImplementANDimmediate) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA & kTestDataI;

    Assembler()
        .LDA().immediate(kTestDataA)
        .AND().immediate(kTestDataI)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate AND and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestDataA}).repeat(6)
                         .signal({kExpectedData}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementANDimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesAND) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestDataA)
            .AND().immediate(kTestDataI)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementEORimmediate) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA ^ kTestDataI;

    Assembler()
        .LDA().immediate(kTestDataA)
        .EOR().immediate(kTestDataI)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate EOR and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestDataA}).repeat(6)
                         .signal({kExpectedData}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementEORimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesEOR) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestDataA)
            .EOR().immediate(kTestDataI)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementORAimmediate) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA | kTestDataI;

    Assembler()
        .LDA().immediate(kTestDataA)
        .ORA().immediate(kTestDataI)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate ORA and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestDataA}).repeat(6)
                         .signal({kExpectedData}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementORAimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesORA) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestDataA)
            .ORA().immediate(kTestDataI)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementANDabsolute) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsolute<AND> testAbsolute = {
        .address = 0x5678,
        .data = kTestDataM,
        .port = o_debug_ac,
        .expected = kTestDataA & kTestDataM,

        .preloadPort = &o_debug_ac,
        .preloadPortValue = kTestDataA
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementANDabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesAND) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestDataA)
            .AND().absolute("M")
            .NOP()
            .org(0x678A)
            .label("M")
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementANDabsoluteIndexedWithXWithoutCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<AND> testAbsolute = {
        {
            .address = 0x5678,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA & kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementANDabsoluteIndexedWithXProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesAND) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kX = 3;
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDX().immediate(kX)
                .AND().absolute("M").x()
                .NOP()
            .org(0x678A)
            .label("M")
            .org(0x678A + kX)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementANDabsoluteIndexedWithXWithCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<AND> testAbsolute = {
        {
            .address = 0x56FE,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA & kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA,
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 3
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementANDabsoluteIndexedWithXProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesAND) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kX = 4;
        
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDX().immediate(kX)
                .AND().absolute("M").x()
                .NOP()
            .org(0xCDFE)
            .label("M")
            .org(0xCDFE + kX)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementANDabsoluteIndexedWithYWithoutCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<AND> testAbsolute = {
        {
            .address = 0x5678,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA & kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementANDabsoluteIndexedWithYProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesAND) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kY = 3;
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDY().immediate(kY)
                .AND().absolute("M").y()
                .NOP()
            .org(0x678A)
            .label("M")
            .org(0x678A + kY)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementANDabsoluteIndexedWithYWithCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<AND> testAbsolute = {
        {
            .address = 0x56FE,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA & kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA,
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 3
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementANDabsoluteIndexedWithYProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesAND) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kY = 4;
        
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDY().immediate(kY)
                .AND().absolute("M").y()
                .NOP()
            .org(0xCDFE)
            .label("M")
            .org(0xCDFE + kY)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementEORabsolute) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsolute<EOR> testAbsolute = {
        .address = 0x5678,
        .data = kTestDataM,
        .port = o_debug_ac,
        .expected = kTestDataA ^ kTestDataM,

        .preloadPort = &o_debug_ac,
        .preloadPortValue = kTestDataA
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementEORabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesEOR) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .EOR().absolute("M")
                .NOP()
            .org(0x678A)
            .label("M")
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementEORabsoluteIndexedWithXWithoutCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<EOR> testAbsolute = {
        {
            .address = 0x5678,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA ^ kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementEORabsoluteIndexedWithXProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesEOR) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kX = 3;
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDX().immediate(kX)
                .EOR().absolute("M").x()
                .NOP()
            .org(0x678A)
            .label("M")
            .org(0x678A + kX)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementEORabsoluteIndexedWithXWithCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<EOR> testAbsolute = {
        {
            .address = 0x56FE,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA ^ kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA,
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 3
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementEORabsoluteIndexedWithXProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesEOR) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kX = 4;
        
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDX().immediate(kX)
                .EOR().absolute("M").x()
                .NOP()
            .org(0xCDFE)
            .label("M")
            .org(0xCDFE + kX)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementEORabsoluteIndexedWithYWithoutCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<EOR> testAbsolute = {
        {
            .address = 0x5678,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA ^ kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementEORabsoluteIndexedWithYProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesEOR) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kY = 3;
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDY().immediate(kY)
                .EOR().absolute("M").y()
                .NOP()
            .org(0x678A)
            .label("M")
            .org(0x678A + kY)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementEORabsoluteIndexedWithYWithCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<EOR> testAbsolute = {
        {
            .address = 0x56FE,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA ^ kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA,
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 3
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementEORabsoluteIndexedWithYProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesEOR) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kY = 4;
        
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDY().immediate(kY)
                .EOR().absolute("M").y()
                .NOP()
            .org(0xCDFE)
            .label("M")
            .org(0xCDFE + kY)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementORAabsolute) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsolute<ORA> testAbsolute = {
        .address = 0x5678,
        .data = kTestDataM,
        .port = o_debug_ac,
        .expected = kTestDataA | kTestDataM,

        .preloadPort = &o_debug_ac,
        .preloadPortValue = kTestDataA
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementORAabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesORA) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .ORA().absolute("M")
                .NOP()
            .org(0x678A)
            .label("M")
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementORAabsoluteIndexedWithXWithoutCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<ORA> testAbsolute = {
        {
            .address = 0x5678,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA | kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementORAabsoluteIndexedWithXProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesORA) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kX = 3;
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDX().immediate(kX)
                .ORA().absolute("M").x()
                .NOP()
            .org(0x678A)
            .label("M")
            .org(0x678A + kX)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}


TEST_F(Cpu6502, ShouldImplementORAabsoluteIndexedWithXWithCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<ORA> testAbsolute = {
        {
            .address = 0x56FE,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA | kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA,
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 3
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementORAabsoluteIndexedWithXProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesORA) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kX = 4;
        
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDX().immediate(kX)
                .ORA().absolute("M").x()
                .NOP()
            .org(0xCDFE)
            .label("M")
            .org(0xCDFE + kX)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementORAabsoluteIndexedWithYWithoutCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<ORA> testAbsolute = {
        {
            .address = 0x5678,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA | kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementORAabsoluteIndexedWithYProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesORA) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kY = 3;
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDY().immediate(kY)
                .ORA().absolute("M").y()
                .NOP()
            .org(0x678A)
            .label("M")
            .org(0x678A + kY)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementORAabsoluteIndexedWithYWithCarry) {
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestDataM = 0b10010110;

    TestAbsoluteIndexed<ORA> testAbsolute = {
        {
            .address = 0x56FE,
            .data = kTestDataM,
            .port = o_debug_ac,
            .expected = kTestDataA | kTestDataM,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestDataA,
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 3
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementORAabsoluteIndexedWithYProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesORA) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kY = 4;
        
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .LDY().immediate(kY)
                .ORA().absolute("M").y()
                .NOP()
            .org(0xCDFE)
            .label("M")
            .org(0xCDFE + kY)
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementORAzeropage) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA | kTestDataI;

    Assembler assembler;
    assembler
        .byte(0)
        .org(0x0000 + kTestAddressZeroPage)
            .byte(kTestDataI)
        .org(1234)
        .label("init")
            .LDA().immediate(kTestDataA)
        .label("start")
            .ORA().zp(kTestAddressZeroPage)
            .NOP()
        .org(0xfffc)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate ORA and NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({
                            addressStart.byteIndex(),
                            addressStart.byteIndex() + 1u,
                            0x0000 + kTestAddressZeroPage,
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u,
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestDataA}).repeat(4).repeatEachStep(2)
                         .signal({kExpectedData}).repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(5).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(5).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementORAzeropageProcessorStatus) {
    for (auto& testCase : kTestCasesORA) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        const uint8_t kTestAddressZeroPage = 0x53;

        sram.clear(0);
    
        Assembler assembler;
        assembler
            .byte(0)
            .org(0x0000 + kTestAddressZeroPage)
                .byte(kTestDataI)
            .org(1234)
            .label("init")
                .CLI()
                .LDA().immediate(kTestDataA)
            .label("start")
                .ORA().zp(kTestAddressZeroPage)
                .NOP()
            .org(0xfffc)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementEORzeropage) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddress = 0x67;
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA ^ kTestDataI;

    Assembler()
            .LDA().immediate(kTestDataA)
            .EOR().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(kTestDataI)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate EOR and NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({
                            // EOR
                            2,
                            3,
                            0x0000 + kTestAddress,

                            // NOP
                            4,
                            5
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestDataA}).repeat(8)
                         .signal({kExpectedData}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(10)
        .port(o_debug_y).signal({0x00}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementEORzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0x83;

    for (auto& testCase : kTestCasesEOR) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .EOR().zp(kTestAddress)
                .NOP()
            .org(0x0000 + kTestAddress)
            .byte(kTestDataI)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementANDzeropage) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddress = 0x78;
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA & kTestDataI;

    Assembler()
            .LDA().immediate(kTestDataA)
            .AND().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(kTestDataI)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate AND and NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({
                            // ND
                            2,
                            3,
                            0x0000 + kTestAddress,

                            // NOP
                            4,
                            5
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestDataA}).repeat(8)
                         .signal({kExpectedData}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(10)
        .port(o_debug_y).signal({0x00}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementANDzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0x96;

    for (auto& testCase : kTestCasesAND) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(kTestDataA)
                .AND().zp(kTestAddress)
                .NOP()
            .org(0x0000 + kTestAddress)
            .byte(kTestDataI)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementORAzeropageIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA | kTestDataI;
    
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
            .ORA().zp(kTestAddressZeroPage).x()
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

    // simulate ORA and NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // ORA
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

TEST_F(Cpu6502, ShouldImplementORAzeropageIndexedWithXProcessorStatus) {
    const uint8_t kTestAddressZeroPage = 0x53;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);

    for (auto& testCase : kTestCasesORA) {
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
                .ORA().zp(kTestAddressZeroPage).x()
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

TEST_F(Cpu6502, ShouldImplementANDzeropageIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA & kTestDataI;
    
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
            .AND().zp(kTestAddressZeroPage).x()
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

    // simulate AND and NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // AND
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

TEST_F(Cpu6502, ShouldImplementANDzeropageIndexedWithXProcessorStatus) {
    const uint8_t kTestAddressZeroPage = 0x53;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);

    for (auto& testCase : kTestCasesAND) {
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
                .AND().zp(kTestAddressZeroPage).x()
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

TEST_F(Cpu6502, ShouldImplementEORzeropageIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);
    const uint8_t kTestDataI = 0b10010110;
    const uint8_t kExpectedData = kTestDataA ^ kTestDataI;
    
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
            .EOR().zp(kTestAddressZeroPage).x()
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

    // simulate EOR and NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // EOR
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

TEST_F(Cpu6502, ShouldImplementEORzeropageIndexedWithXProcessorStatus) {
    const uint8_t kTestAddressZeroPage = 0x53;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);

    for (auto& testCase : kTestCasesEOR) {
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
                .EOR().zp(kTestAddressZeroPage).x()
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

TEST_F(Cpu6502, ShouldImplementORAZeroPageIndirectIndexedWithY) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0x4;
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x33;
    const uint8_t kExpected = uint8_t(kTestPreloadA | kTestData);

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
            .ORA().zpIndirect(kTestAddressIndirect).y()
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

    // simulate ORA (zp),y
    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // ORA (zp),y
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

TEST_F(Cpu6502, ShouldImplementORAZeroPageIndirectIndexedWithYWithCarry) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0xF4;
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x35;
    const uint8_t kExpected = uint8_t(kTestPreloadA | kTestData);

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
            .ORA().zpIndirect(kTestAddressIndirect).y()
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

    // simulate ORA (zp),y
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // ORA (zp),y
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

TEST_F(Cpu6502, ShouldImplementORAZeroPageIndirectIndexedWithYProcessorStatus) {
    for (auto& testCase : kTestCasesORA) {
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
                .ORA().zpIndirect(kTestAddressIndirect).y()
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

TEST_F(Cpu6502, ShouldImplementANDZeroPageIndirectIndexedWithY) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0x4;
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x33;
    const uint8_t kExpected = uint8_t(kTestPreloadA & kTestData);

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
            .AND().zpIndirect(kTestAddressIndirect).y()
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

    // simulate AND (zp),y
    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // AND (zp),y
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

TEST_F(Cpu6502, ShouldImplementANDZeroPageIndirectIndexedWithYWithCarry) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0xF4;
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x35;
    const uint8_t kExpected = uint8_t(kTestPreloadA & kTestData);

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
            .AND().zpIndirect(kTestAddressIndirect).y()
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

    // simulate AND (zp),y
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // AND (zp),y
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

TEST_F(Cpu6502, ShouldImplementANDZeroPageIndirectIndexedWithYProcessorStatus) {
    for (auto& testCase : kTestCasesAND) {
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
                .AND().zpIndirect(kTestAddressIndirect).y()
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

TEST_F(Cpu6502, ShouldImplementEORZeroPageIndirectIndexedWithY) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0x4;
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x33;
    const uint8_t kExpected = uint8_t(kTestPreloadA ^ kTestData);

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
            .EOR().zpIndirect(kTestAddressIndirect).y()
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

    // simulate EOR (zp),y
    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // EOR (zp),y
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

TEST_F(Cpu6502, ShouldImplementEORZeroPageIndirectIndexedWithYWithCarry) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0xF4;
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x35;
    const uint8_t kExpected = uint8_t(kTestPreloadA ^ kTestData);

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
            .EOR().zpIndirect(kTestAddressIndirect).y()
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

    // simulate EOR (zp),y
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // EOR (zp),y
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

TEST_F(Cpu6502, ShouldImplementEORZeroPageIndirectIndexedWithYProcessorStatus) {
    for (auto& testCase : kTestCasesEOR) {
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
                .EOR().zpIndirect(kTestAddressIndirect).y()
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

TEST_F(Cpu6502, ShouldImplementORAZeroPageIndirectIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kTestOffset = 0x40;
    const uint16_t kTestAddressZeroPageWithOffset = 0x0000 + uint8_t(kTestAddressZeroPage + kTestOffset);
    const uint16_t kTestAddress = 0x1230;    
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x33;
    const uint8_t kExpected = uint8_t(kTestPreloadA | kTestData);

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
            .ORA().zpIndirect(kTestAddressZeroPage).x()
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

    // simulate ORA (zp),x
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // ORA (zp),x
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

TEST_F(Cpu6502, ShouldImplementORAZeroPageIndirectIndexedWithXProcessorStatus) {
    for (auto& testCase : kTestCasesORA) {
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
                .ORA().zpIndirect(kTestAddressZeroPage).x()
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
