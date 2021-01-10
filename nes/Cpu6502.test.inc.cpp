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
