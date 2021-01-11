#include "Cpu6502.fixture.h"

TEST_F(Cpu6502, ShouldImplementLSRaccumulator) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x93;

    Assembler()
        .LDA().immediate(kTestData)
        .LSR().A()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate LSR and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(6)
                        .signal({kTestData >> 1}).repeat(2)
        .port(o_debug_x).signal({0xFF})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLSRProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {1, Z | C},
        {3, C},
        {0, 0},
        {0xFF, C}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
        .LDA().immediate(kTestData)
        .LSR().A()
        .NOP()
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}