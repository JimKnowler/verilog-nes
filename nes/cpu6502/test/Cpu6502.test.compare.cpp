#include "Cpu6502.fixture.hpp"

namespace {
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> kTestCasesCMP = {
        {{0x00, 0x00}, C|Z},
        {{0x01, 0x00}, C},
        {{0x80, 0x00}, C|N},
        {{0x00, 0x01}, N}
    };

    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> kTestCasesBIT = {
        {{0x00, 0x00}, Z},          // A == M (==0)
        {{0x00, 0x80}, Z|N},        // load N from M[6]
        {{0x00, 0x40}, Z|V},        // load V from M[7]
        {{0x01, 0x01}, 0},          // A == M
        {{0x01, 0x02}, Z},          // A != M
        {{0x40, 0x40}, V},
        {{0x80, 0x80}, N},
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
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCMPimmediateProcessorStatus) {
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> testCases = {
        {{0x00, 0x00}, C|Z},
        {{0x01, 0x00}, C},
        {{0x80, 0x00}, C|N},
        {{0x00, 0x01}, N}
    };

    for (auto& testCase : testCases) {
        const uint8_t A = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
            .LDA().immediate(A)
            .CMP().immediate(M)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
        .port(o_debug_ac).signal({0xFF}).repeat(8)
        .port(o_debug_x).signal({X}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCPXimmediateProcessorStatus) {
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> testCases = {
        {{0x00, 0x00}, C|Z},
        {{0x01, 0x00}, C},
        {{0x80, 0x00}, C|N},
        {{0x00, 0x01}, N}
    };

    for (auto& testCase : testCases) {
        const uint8_t X = testCase.first.first;
        const uint8_t M = testCase.first.second;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
            .LDX().immediate(X)
            .CPX().immediate(M)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
        .port(o_debug_ac).signal({0xFF}).repeat(8)
        .port(o_debug_x).signal({0xFF}).repeat(8)
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
            .LDY().immediate(Y)
            .CPY().immediate(M)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
            .LDA().immediate(A)
            .BIT().absolute(ADDRESS)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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
                .LDA().immediate(A)
                .CMP().absolute("M")
                .NOP()
            .org(0x4567)
            .label("M")
            .byte(M)    
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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
                .LDX().immediate(X)
                .CPX().absolute("compare_to")
                .NOP()
            .org(0x4567)
            .label("compare_to")
            .byte(M)    
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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
                .LDY().immediate(Y)
                .CPY().absolute("compare_to")
                .NOP()
            .org(0x4567)
            .label("compare_to")
            .byte(M)    
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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

        testBench.tick(10);
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

        testBench.tick(11);
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

        testBench.tick(10);
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

        testBench.tick(11);
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
        .port(o_debug_x).signal({0xFF}).repeat(5).repeatEachStep(2)
        .port(o_debug_y).signal({0xFF}).repeat(5).repeatEachStep(2);

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
                .LDA().immediate(A)
            .label("start")
                .BIT().zp(kTestAddressZeroPage)
                .NOP()
            .org(0xfffc)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(7);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}