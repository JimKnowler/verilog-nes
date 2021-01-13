#include "Cpu6502.fixture.h"

TEST_F(Cpu6502, ShouldImplementSEC) {
    sram.clear(0);

    Assembler()
        .SEC()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // simulate SEC and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(8)
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8)
        .port(o_debug_p).signal({0}).repeat(4)
                        .signal({C}).repeat(4);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCLC) {
sram.clear(0);

    Assembler()
        .SEC()
        .CLC()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip SEC
    testBench.tick(2);
    testBench.trace.clear();

    // simulate SEC and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({1, 2, 2, 3})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(8)
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8)
        .port(o_debug_p).signal({C}).repeat(4)
                        .signal({0}).repeat(4);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}