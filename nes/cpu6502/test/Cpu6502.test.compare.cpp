#include "Cpu6502.fixture.hpp"

namespace {
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> kTestCasesCMP = {
        {{0x00, 0x00}, U|C|Z},
        {{0x01, 0x00}, U|C},
        {{0x80, 0x00}, U|C|N},
        {{0x00, 0x01}, U|N},
        {{0x40, 0x40}, U|C|Z}
    };

    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> kTestCasesBIT = {
        {{0x00, 0x00}, U|Z},          // A == M (==0)
        {{0x00, 0x80}, U|Z|N},        // load N from M[6]
        {{0x00, 0x40}, U|Z|V},        // load V from M[7]
        {{0x01, 0x01}, U},          // A == M
        {{0x01, 0x02}, U|Z},          // A != M
        {{0x40, 0x40}, U|V},
        {{0x80, 0x80}, U|N},
    };
}

TEST_F(Cpu6502, ShouldImplementCMPimmediate) {
    sram.clear(0);
    
    const uint8_t A = 0x45;
    const uint8_t M = 0x22;

    Assembler()
        .LDA().immediate(A)
        .CMP().immediate(M)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate
    testBench.tick(2);
    testBench.trace.clear();

    // simulte CMP + NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({A}).repeat(8)
        .port(o_debug_x).signal({0}).repeat(8)
        .port(o_debug_y).signal({0}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCMPimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesCMP) {
        const uint8_t A = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(A)
            .CMP().immediate(M)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCPXimmediate) {
    sram.clear(0);
    
    const uint8_t X = 0x45;
    const uint8_t M = 0x22;

    Assembler()
        .LDX().immediate(X)
        .CPX().immediate(M)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDXimmediate
    testBench.tick(2);
    testBench.trace.clear();

    // simulate CPX + NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(8)
        .port(o_debug_x).signal({X}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCPXimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesCMP) {
        const uint8_t X = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(X)
            .CPX().immediate(M)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCPYimmediate) {
    sram.clear(0);
    
    const uint8_t Y = 0x45;
    const uint8_t M = 0x22;

    Assembler()
        .LDY().immediate(Y)
        .CPY().immediate(M)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDYimmediate
    testBench.tick(2);
    testBench.trace.clear();

    // simulate CPY + NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({Y}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCPYimmediateProcessorStatus) {
    for (auto& testCase : kTestCasesCMP) {
        const uint8_t Y = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDY().immediate(Y)
            .CPY().immediate(M)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementBITabsolute) { 
    const uint8_t A = 0x23;
    const uint16_t ADDRESS = 0xAABB;
    const uint8_t M = 0x26;

    TestAbsolute<BIT> testAbsolute {
        .address = ADDRESS,
        .data = M,
        .preloadPort = &o_debug_ac,
        .preloadPortValue = A
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementBITabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesBIT) {
        const uint8_t A = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        const uint16_t ADDRESS = 0xAABB;

        sram.clear(0);
        sram.write(ADDRESS, M);
    
        Assembler()
            .CLI()
            .LDA().immediate(A)
            .BIT().absolute(ADDRESS)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCMPabsolute) {
    const uint8_t A = 0x45;
    const uint8_t M = 0x22;

    TestAbsolute<CMP> testAbsolute = {
        .address = 0x5678,
        .data = M,
        .port = o_debug_ac,
        .expected = A,

        .preloadPort = &o_debug_ac,
        .preloadPortValue = A
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementCMPabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesCMP) {
        const uint8_t A = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(A)
                .CMP().absolute("M")
                .NOP()
            .org(0x4567)
            .label("M")
            .byte(M)    
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCPXabsolute) {
    const uint8_t X = 0x45;
    const uint8_t M = 0x22;

    TestAbsolute<CPX> testAbsolute = {
        .address = 0x5678,
        .data = M,
        .port = o_debug_x,
        .expected = X,

        .preloadPort = &o_debug_x,
        .preloadPortValue = X
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementCPXabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesCMP) {
        const uint8_t X = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDX().immediate(X)
                .CPX().absolute("compare_to")
                .NOP()
            .org(0x4567)
            .label("compare_to")
            .byte(M)    
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCPYabsolute) {
    const uint8_t Y = 0x45;
    const uint8_t M = 0x22;

    TestAbsolute<CPY> testAbsolute = {
        .address = 0x5678,
        .data = M,
        .port = o_debug_y,
        .expected = Y,

        .preloadPort = &o_debug_y,
        .preloadPortValue = Y
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementCPYabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesCMP) {
        const uint8_t Y = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDY().immediate(Y)
                .CPY().absolute("compare_to")
                .NOP()
            .org(0x4567)
            .label("compare_to")
            .byte(M)    
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCMPabsoluteIndexedWithXWithoutCarry) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t A = 0x45;
    const uint8_t M = 0x22;

    TestAbsoluteIndexed<CMP> testAbsolute = {
        {
            .address = kTestAddress,
            .data = M,
            .port = o_debug_ac,
            .expected = A,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = A
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementCMPabsoluteIndexedWithXProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesCMP) {
        const uint8_t A = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kX = 3;
    
        Assembler()
                .CLI()
                .LDA().immediate(A)
                .LDX().immediate(kX)
                .CMP().a("M").x()
                .NOP()
            .org(0x678A)
            .label("M")
            .org(0x678A + kX)
            .byte(M)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCMPabsoluteIndexedWithXWithCarry) {
    const uint16_t kTestAddress = 0x56FE;
    const uint8_t A = 0x45;
    const uint8_t M = 0x22; 

    TestAbsoluteIndexed<CMP> testAbsolute = {
        {
            .address = kTestAddress,
            .data = M,
            .port = o_debug_ac,
            .expected = A,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = A
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 3,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementCMPabsoluteIndexedWithXProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesCMP) {
        const uint8_t A = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kX = 4;
    
        Assembler()
                .CLI()
                .LDA().immediate(A)
                .LDX().immediate(kX)
                .CMP().a("M").x()
                .NOP()
            .org(0xCDFE)
            .label("M")
            .org(0xCDFE + kX)
            .byte(M)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCMPabsoluteIndexedWithYWithoutCarry) {
    const uint16_t kTestAddress = 0x56FE;
    const uint8_t A = 0x45;
    const uint8_t M = 0x22;

    TestAbsoluteIndexed<CMP> testAbsolute = {
        {
            .address = kTestAddress,
            .data = M,
            .port = o_debug_ac,
            .expected = A,
            
            .preloadPort = &o_debug_ac,
            .preloadPortValue = A
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementCMPabsoluteIndexedWithYProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesCMP) {
        const uint8_t A = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kY = 3;
    
        Assembler()
                .CLI()
                .LDA().immediate(A)
                .LDY().immediate(kY)
                .CMP().a("M").y()
                .NOP()
            .org(0x678A)
            .label("M")
            .org(0x678A + kY)
            .byte(M)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCMPabsoluteIndexedWithYWithCarry) {
    const uint16_t kTestAddress = 0x56FE;
    const uint8_t A = 0x45;
    const uint8_t M = 0x22;

    TestAbsoluteIndexed<CMP> testAbsolute = {
        {
            .address = kTestAddress,
            .data = M,
            .port = o_debug_ac,
            .expected = A,

            .preloadPort = &o_debug_ac,
            .preloadPortValue = A
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 3,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementCMPabsoluteIndexedWithYProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesCMP) {
        const uint8_t A = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kY = 4;
    
        Assembler()
                .CLI()
                .LDA().immediate(A)
                .LDY().immediate(kY)
                .CMP().a("M").y()
                .NOP()
            .org(0xCDFE)
            .label("M")
            .org(0xCDFE + kY)
            .byte(M)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(13);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementBITzeropage) { 
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kTestDataI = 0b10010110;

    Assembler assembler;
    assembler
        .byte(0)
        .org(0x0000 + kTestAddressZeroPage)
            .byte(kTestDataI)
        .org(1234)
        .label("init")
            .LDA().immediate(kTestDataA)
        .label("start")
            .BIT().zp(kTestAddressZeroPage)
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

    // simulate BIT and NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({
                            //BIT
                            addressStart.byteIndex(),
                            addressStart.byteIndex() + 1u,
                            0x0000 + kTestAddressZeroPage,

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u,
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestDataA}).repeat(5).repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(5).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(5).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBITzeropageProcessorStatus) {
    const uint8_t kTestAddressZeroPage = 0x48;

    for (auto& testCase : kTestCasesBIT) {
        const uint8_t A = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .byte(0)
            .org(0x0000 + kTestAddressZeroPage)
                .byte(M)
            .org(1234)
            .label("init")
                .CLI()
                .LDA().immediate(A)
            .label("start")
                .BIT().zp(kTestAddressZeroPage)
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

TEST_F(Cpu6502, ShouldImplementCMPzeropage) {
    sram.clear(0);
    
    const uint8_t A = 0x45;
    const uint8_t M = 0x22;
    const uint8_t kTestAddress = 0x94;

    Assembler()
            .LDA().immediate(A)
            .CMP().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(M)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate
    testBench.tick(2);
    testBench.trace.clear();
    
    // simulate CMP & NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({2, 3, kTestAddress, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(10)
        .port(o_debug_ac).signal({A}).repeat(10)
        .port(o_debug_y).signal({0x00}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCMPzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0x78;

    for (auto& testCase : kTestCasesCMP) {
        const uint8_t A = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().immediate(A)
                .CMP().zp(kTestAddress)
                .NOP()
            .org(0x0000 + kTestAddress)
            .byte(M)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCPXzeropage) {
    sram.clear(0);
    
    const uint8_t X = 0x45;
    const uint8_t M = 0x22;
    const uint8_t kTestAddress = 0x94;

    Assembler()
            .LDX().immediate(X)
            .CPX().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(M)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate
    testBench.tick(2);
    testBench.trace.clear();
    
    // simulate CMP & NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({2, 3, kTestAddress, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(10)
        .port(o_debug_x).signal({X}).repeat(10)
        .port(o_debug_y).signal({0x00}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCPXzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0x78;

    for (auto& testCase : kTestCasesCMP) {
        const uint8_t X = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDX().immediate(X)
                .CPX().zp(kTestAddress)
                .NOP()
            .org(0x0000 + kTestAddress)
            .byte(M)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCPYzeropage) {
    sram.clear(0);
    
    const uint8_t Y = 0x45;
    const uint8_t M = 0x22;
    const uint8_t kTestAddress = 0x94;

    Assembler()
            .LDY().immediate(Y)
            .CPY().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(M)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAimmediate
    testBench.tick(2);
    testBench.trace.clear();
    
    // simulate CMP & NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({2, 3, kTestAddress, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(10)
        .port(o_debug_x).signal({0x00}).repeat(10)
        .port(o_debug_y).signal({Y}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCPYzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0x78;

    for (auto& testCase : kTestCasesCMP) {
        const uint8_t Y = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDY().immediate(Y)
                .CPY().zp(kTestAddress)
                .NOP()
            .org(0x0000 + kTestAddress)
            .byte(M)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementCMPzeropageIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestDataA = 0b10110011;
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);
    const uint8_t kTestDataI = 0b10010110;
    
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
            .CMP().zp(kTestAddressZeroPage).x()
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

    // simulate CMP and NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // CMP
                            addressStart.byteIndex(),
                            addressStart.byteIndex() + 1u,
                            0x0000 + kTestAddressZeroPage,
                            kTestAddressIndexed,

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u,
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestDataA}).repeat(6).repeatEachStep(2)
        .port(o_debug_x).signal({kX}).repeat(6).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(6).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCMPzeropageIndexedWithXProcessorStatus) {
    const uint8_t kTestAddressZeroPage = 0x53;
    const uint8_t kX = 0xFE;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddressZeroPage + kX) % 0x0100);

    for (auto& testCase : kTestCasesCMP) {
        const uint8_t kTestDataA = testCase.first.first;
        const uint8_t kTestDataM = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler assembler;
        assembler
            .byte(0)
            .org(kTestAddressIndexed)
                .byte(kTestDataM)
            .org(1234)
            .label("init")
                .CLI()
                .LDX().immediate(kX)
                .LDA().immediate(kTestDataA)
            .label("start")
                .CMP().zp(kTestAddressZeroPage).x()
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

TEST_F(Cpu6502, ShouldImplementCMPZeroPageIndirectIndexedWithY) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0x4;
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x33;

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
            .CMP().zpIndirect(kTestAddressIndirect).y()
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

    // simulate CMP (zp),y
    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // CMP (zp),y
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
        .port(o_debug_ac).signal({kTestPreloadA}).repeat(7)
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({kTestOffset}).repeat(7).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCMPZeroPageIndirectIndexedWithYWithCarry) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0xF4;
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x35;

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
            .CMP().zpIndirect(kTestAddressIndirect).y()
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

    // simulate CMP (zp),y
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // CMP (zp),y
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
        .port(o_debug_ac).signal({kTestPreloadA}).repeat(8)
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(8).repeatEachStep(2)
        .port(o_debug_y).signal({kTestOffset}).repeat(8).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCMPZeroPageIndirectIndexedWithYProcessorStatus) {
    for (auto& testCase : kTestCasesCMP) {
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
                .CMP().zpIndirect(kTestAddressIndirect).y()
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

TEST_F(Cpu6502, ShouldImplementCMPZeroPageIndirectIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kTestOffset = 0x40;
    const uint16_t kTestAddressZeroPageWithOffset = 0x0000 + uint8_t(kTestAddressZeroPage + kTestOffset);
    const uint16_t kTestAddress = 0x1230;    
    const uint8_t kTestData = 0x50;
    const uint8_t kTestPreloadA = 0x33;

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
            .CMP().zpIndirect(kTestAddressZeroPage).x()
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

    // simulate CMP (zp,x)
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // CMP (zp,x)
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
        .port(o_debug_ac).signal({kTestPreloadA}).repeat(8)
                        .repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(8).repeatEachStep(2)
        .port(o_debug_x).signal({kTestOffset}).repeat(8).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCMPZeroPageIndirectIndexedWithXProcessorStatus) {
    for (auto& testCase : kTestCasesCMP) {
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
                .CMP().zpIndirect(kTestAddressZeroPage).x()
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