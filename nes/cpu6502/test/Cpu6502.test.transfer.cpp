#include "Cpu6502.fixture.hpp"

TEST_F(Cpu6502, ShouldImplementTAX) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x75;

    Assembler()
        .LDA().immediate(kTestData)
        .TAX()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate TAX and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData})
                        .repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(4)
                        .signal({kTestData}).repeat(4)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTAXProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0, U|Z},
        {(1<<7), U|N},
        {1, U}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData)
            .LDY().immediate(1)                 // clear N and Z flags
            .TAX()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementTAY) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x75;

    Assembler()
        .LDA().immediate(kTestData)
        .TAY()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate TAY and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData})
                        .repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(4)
                        .signal({kTestData}).repeat(4);
        
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTAYProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0, U|Z},
        {(1<<7), U|N},
        {1, U}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData)
            .LDX().immediate(1)                 // clear N and Z flags
            .TAY()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementTXA) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x73;

    Assembler()
        .LDX().immediate(kTestData)
        .TXA()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDX
    testBench.tick(2);
    testBench.trace.clear();

    // simulate TXA and NOP
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
                        .signal({kTestData}).repeat(4)
        .port(o_debug_x).signal({kTestData})
                        .repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTXAProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0, U|Z},
        {(1<<7), U|N},
        {1, U}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(kTestData)
            .LDY().immediate(1)                 // clear N and Z flags
            .TXA()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementTYA) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x92;

    Assembler()
        .LDY().immediate(kTestData)
        .TYA()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDY
    testBench.tick(2);
    testBench.trace.clear();

    // simulate TYA and NOP
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
                        .signal({kTestData}).repeat(4)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({kTestData})
                        .repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTYAProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0, U|Z},
        {(1<<7), U|N},
        {1, U}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDY().immediate(kTestData)
            .LDX().immediate(1)                 // clear N and Z flags
            .TYA()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementTXS) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x22;

    Assembler()
        .LDX().immediate(kTestData)
        .TXS()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDX
    testBench.tick(2);
    testBench.trace.clear();

    // simulate TXS and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(8)
        .port(o_debug_x).signal({kTestData})
                        .repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8)
        .port(o_debug_s).signal({0xFC}).repeat(4)
                        .signal({kTestData}).repeat(4);
        

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTXSProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0, U},
        {(1<<7), U},
        {1, U}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(kTestData)
            .LDY().immediate(1)                 // clear N and Z flags
            .TXS()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementTSX) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x22;

    Assembler()
        .LDX().immediate(kTestData)
        .TXS()
        .LDX().immediate(kTestData+1)
        .TSX()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDX, TXS, LDX
    testBench.tick(6);
    testBench.trace.clear();

    // simulate TSX and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({5, 6, 6, 7})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8)
        .port(o_debug_x).signal({kTestData+1}).repeat(4)
                        .signal({kTestData}).repeat(4)
        .port(o_debug_s).signal({kTestData})
                        .repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTSXProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0, U|Z},
        {(1<<7), U|N},
        {1, U}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(kTestData)
            .TXS()
            .LDX().immediate(1)                 // clear N and Z flags
            .TSX()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}
