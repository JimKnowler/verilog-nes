#include "Cpu6502.fixture.hpp"

namespace {
    const std::map<std::pair<uint8_t,uint8_t>, uint8_t> kTestCasesORA = {
        {{0, 0}, Z},
        {{0xff, 0xff}, N},
        {{0x80, 0x40}, N},
        {{0x0, 0x80}, N},
        {{0x01, 0x02}, 0},
        {{0x00, 0x01}, 0}
    };

    const std::map<std::pair<uint8_t,uint8_t>, uint8_t> kTestCasesAND = {
        {{1, 0}, Z},
        {{0x80, 0x80}, N},
        {{1, 1}, 0}
    };

    const std::map<std::pair<uint8_t,uint8_t>, uint8_t> kTestCasesEOR = {
        {{0, 0}, Z},
        {{0xff, 0xff}, Z},
        {{0x80, 0x40}, N},
        {{0x0, 0x80}, N},
        {{0x01, 0x02}, 0}
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
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementANDimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesAND) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDA().immediate(kTestDataA)
            .AND().immediate(kTestDataI)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementEORimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesEOR) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDA().immediate(kTestDataA)
            .EOR().immediate(kTestDataI)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementORAimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesORA) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataI = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDA().immediate(kTestDataA)
            .ORA().immediate(kTestDataI)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
            .LDA().immediate(kTestDataA)
            .AND().absolute("M")
            .NOP()
            .org(0x678A)
            .label("M")
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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

        testBench.tick(10);
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

        testBench.tick(11);
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

        testBench.tick(10);
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

        testBench.tick(11);
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
                .LDA().immediate(kTestDataA)
                .EOR().absolute("M")
                .NOP()
            .org(0x678A)
            .label("M")
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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

        testBench.tick(10);
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

        testBench.tick(11);
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

        testBench.tick(10);
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

        testBench.tick(11);
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
                .LDA().immediate(kTestDataA)
                .ORA().absolute("M")
                .NOP()
            .org(0x678A)
            .label("M")
            .byte(kTestDataM)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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

        testBench.tick(10);
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

        testBench.tick(11);
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

        testBench.tick(10);
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

        testBench.tick(11);
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
        .port(o_debug_x).signal({0xFF}).repeat(5).repeatEachStep(2)
        .port(o_debug_y).signal({0xFF}).repeat(5).repeatEachStep(2);

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
                .LDA().immediate(kTestDataA)
            .label("start")
                .ORA().zp(kTestAddressZeroPage)
                .NOP()
            .org(0xfffc)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}
