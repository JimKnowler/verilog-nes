#include "Cpu6502.fixture.h"

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
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> testCases = {
        {{0x00, 0x00}, C|Z},
        {{0x01, 0x00}, C},
        {{0x80, 0x00}, C|N},
        {{0x00, 0x01}, N}
    };

    for (auto& testCase : testCases) {
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
        .preloadValue = A
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementBITabsoluteProcessorStatus) {
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> testCases = {
        {{0x00, 0x00}, Z},          // A == M (==0)
        {{0x00, 0x80}, Z|N},        // load N from M[6]
        {{0x00, 0x40}, Z|V},        // load V from M[7]
        {{0x01, 0x01}, 0},          // A == M
        {{0x01, 0x02}, Z},          // A != M
        {{0x40, 0x40}, V},
        {{0x80, 0x80}, N},
    };

    for (auto& testCase : testCases) {
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
