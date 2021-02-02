#include "Cpu6502.fixture.h"

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
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF, 0xFF})
                        .repeatEachStep(4)
        .port(o_debug_x).signal({kTestData}).repeat(6)
                        .signal({kTestData+1}).repeat(2)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementINXProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0 - 1, Z},
        {(1<<7) - 1, N},
        {1 - 1, 0}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDX().immediate(kTestData)
            .INX()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF, 0xFF})
                        .repeatEachStep(4)
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({kTestData}).repeat(6)
                        .signal({kTestData+1}).repeat(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementINYProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0 - 1, Z},
        {(1<<7) - 1, N},
        {1 - 1, 0}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDY().immediate(kTestData)
            .INY()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData1}).repeat(6)
                         .signal({kTestData1+kTestData2}).repeat(2)
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);
        

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementADCimmediateProcessorStatus) {
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> testCases = {
        {{0x00, 0x00}, Z},
        {{0x00, 0x01}, 0},
        {{0x7F, 0x01}, N|V},
        {{0x80, 0x00}, N},
        {{0xFF, 0x01}, C|Z},
        {{0xFF, 0x80}, C|V}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDA().immediate(kTestData1)
            .ADC().immediate(kTestData2)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({3, 4, 5, 6})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData1}).repeat(6)
                         .signal({kTestData1+kTestData2+1}).repeat(2)
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementADCimmediateProcessorStatusWithCarryIn) {
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> testCases = {
        {{0x00, 0x00}, 0},
        {{0x00, 0x01}, 0},
        {{0x7F, 0x00}, N|V},
        {{0x80, 0x00}, N},
        {{0xFF, 0x00}, C|Z},
        {{0xFF, 0x01}, C},
        {{0xFF, 0x80}, C|N},
        {{0xFF, 0x7F}, C}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData1 = testCase.first.first;
        const uint8_t kTestData2 = testCase.first.second;
        
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDA().immediate(kTestData1)
            .SEC()
            .ADC().immediate(kTestData2)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementINCabsolute) {
    sram.clear(0);
    
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x49;

    Assembler assembler;
    assembler
            .INC().absolute(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

    cpu6502::assembler::Address addressTestData;
    assembler.lookupAddress(addressTestData);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11110011")
                    .repeatEachStep(2)
        .port(o_sync).signal("01000001").repeatEachStep(2)
        .port(o_data).signal({0}).repeat(9)
                     .signal({kTestData}).repeat(2)
                     .signal({kTestData + 1}).repeat(2)
                     .signal({0}).repeat(3)
        .port(o_address).signal({
                            // INC
                            0,
                            1,
                            2,
                            kTestAddress,
                            kTestAddress,
                            kTestAddress,
                            // NOP
                            3,
                            4
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF})
                        .repeat(16)
        .port(o_debug_x).signal({0xFF})
                        .repeat(16)
        .port(o_debug_y).signal({0xFF})
                        .repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementINCabsoluteProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0 - 1, Z},
        {(1<<7) - 1, N},
        {1 - 1, 0}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint16_t kTestAddress = 0x5678;
        
        Assembler assembler;
        assembler
                .INC().absolute(kTestAddress)
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
