#include "Cpu6502.fixture.hpp"

TEST_F(Cpu6502, ShouldImplementLDAi) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x53;

    Assembler()
        .LDA().immediate(kTestData)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, 3})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF, kTestData})
                        .repeatEachStep(4)
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDAiProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0x00, Z},
        {1<<7, N},
        {1, 0}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDA().immediate(kTestData)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(3);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDAa) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x42;

    TestAbsolute<LDA> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .port = o_debug_ac,
        .expected = kTestData
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLDAaProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0x00, Z},
        {1<<7, N},
        {1, 0}
    };

    for (auto& testCase : testCases) {
        const uint16_t kTestAddress = 0x5678;
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
        
        Assembler()
            .LDA().a(kTestAddress)
            .NOP()
            .compileTo(sram);

        sram.write(kTestAddress, kTestData);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}
