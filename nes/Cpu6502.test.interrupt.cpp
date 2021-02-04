#include "Cpu6502.fixture.h"

TEST_F(Cpu6502, ShouldImplementBRK) {
    const auto& core = testBench.core();

    sram.clear(0);

    Assembler assembler;
    assembler
            .NOP()
        .org(0x1234)
        .label("init")
            .SEC()                  // non-zero Processor Status (P)
            .SEI()
        .label("start")
            .BRK()
        .label("return_to")
        .org(0x3456)
        .label("interrupt")
            .NOP()
        .org(0xfffc)                // RESET VECTOR
        .word("init")
        .org(0xFFFE)                // INTERRUPT VECTOR
        .word("interrupt")
        .compileTo(sram);

    helperSkipResetVector();

    // skip init section
    testBench.tick(4);
    testBench.trace.clear();

    // capture information from the assembler / simulated core
    cpu6502::assembler::Address start("start");
    assembler.lookupAddress(start);

    cpu6502::assembler::Address interrupt("interrupt");
    assembler.lookupAddress(interrupt);

    cpu6502::assembler::Address returnTo("return_to");
    assembler.lookupAddress(returnTo);

    const uint8_t sp = core.o_debug_s;
    const uint8_t p = C | I;

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(9)
        .port(o_rw).signal("110001111").repeatEachStep(2)
        .port(o_sync).signal("010000001").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(5)
            .signal({returnTo.hi()}).repeat(2)
            .signal({returnTo.lo()}).repeat(2)
            .signal({p}).repeat(2)
            .signal({0}).repeat(7)
        .port(o_address)
            .signal({
                // BRK - at 'start'
                start.byteIndex(),
                start.byteIndex() + 1u,
                0x0100u + sp,
                0x0100u + sp - 1u,
                0x0100u + sp - 2u,
                0xFFFE,                     // Interrupt Vector (low byte)
                0xFFFF,                     // Interrupt Vector (high byte)

                // NOP (at interrupt vector)
                interrupt.byteIndex(),
                interrupt.byteIndex() + 1u
            }).repeatEachStep(2)
        .port(o_debug_s)
            .signal({sp}).repeat(6)
            .signal({sp - 3u}).repeat(3)
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    // processor flags should be unaffected, apart from B=Break
    EXPECT_EQ(p | B, core.o_debug_p);
}