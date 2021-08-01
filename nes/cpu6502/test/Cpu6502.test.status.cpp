#include "Cpu6502.fixture.hpp"

TEST_F(Cpu6502, ShouldImplementSEC) {
    sram.clear(0);

    Assembler()
        .CLI()
        .SEC()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip CLI
    testBench.tick(2);
    testBench.trace.clear();

    // simulate SEC and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({1, 2, 2, 3})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8)
        .port(o_debug_p).signal({U}).repeat(4)
                        .signal({U|C}).repeat(4);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCLC) {
sram.clear(0);

    Assembler()
        .CLI()
        .SEC()
        .CLC()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip CLI, SEC
    testBench.tick(4);
    testBench.trace.clear();

    // simulate CLC and NOP
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
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8)
        .port(o_debug_p).signal({U|C}).repeat(4)
                        .signal({U}).repeat(4);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSEI) {
    sram.clear(0);

    Assembler()
        .CLI()
        .SEI()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip CLI
    testBench.tick(2);
    testBench.trace.clear();

    // simulate
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({1, 2, 2, 3})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8)
        .port(o_debug_p).signal({U}).repeat(4)
                        .signal({U|I}).repeat(4);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCLI) {
    sram.clear(0);

    Assembler()
        .SEI()
        .CLI()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip SEI
    testBench.tick(2);
    testBench.trace.clear();

    // simulate CLI and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({1, 2, 2, 3})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8)
        .port(o_debug_p).signal({U|I}).repeat(4)
                        .signal({U}).repeat(4);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCLV) {
    sram.clear(0);

    const uint8_t kBefore = U|C|Z|I|D|V|N;          //NOTE: B is not used in status register
    const uint8_t kAfter = kBefore & ~V;

    Assembler()
        .PLP()
        .CLV()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    sram.write(0x100 | testBench.core().o_debug_s + 1, kBefore);

    // skip PLV
    testBench.tick(4);
    testBench.trace.clear();

    // simulate CLV and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({1, 2, 2, 3})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8)
        .port(o_debug_p).signal({kBefore}).repeat(4)
                        .signal({kAfter}).repeat(4);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementSED) {
    sram.clear(0);

    Assembler()
        .CLI()
        .SED()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip CLI
    testBench.tick(2);
    testBench.trace.clear();

    // simulate SED and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({1, 2, 2, 3})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8)
        .port(o_debug_p).signal({U}).repeat(4)
                        .signal({U|D}).repeat(4);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementCLD) {
sram.clear(0);

    Assembler()
        .CLI()
        .SED()
        .CLD()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip CLI, SED
    testBench.tick(4);
    testBench.trace.clear();

    // simulate CLD and NOP
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
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8)
        .port(o_debug_p).signal({U|D}).repeat(4)
                        .signal({U}).repeat(4);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
