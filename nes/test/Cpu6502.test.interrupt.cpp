#include "Cpu6502.fixture.hpp"

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
        .org(0xFFFC)                // RESET VECTOR
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
        .port(o_sync).signal("100000010").repeatEachStep(2)
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
            .signal({sp}).repeat(5)
            .signal({sp - 3u}).repeat(4)
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    // processor flags should be unaffected, apart from B=Break
    EXPECT_EQ(p | B, core.o_debug_p);
}

TEST_F(Cpu6502, ShouldImplementRTI) {
    const auto& core = testBench.core();

    sram.clear(0);

    Assembler assembler;
    assembler
            .NOP()
        .org(0x1234)
        .label("init")
            .SEC()                  // non-zero Processor Status (P)
            .SEI()                  // P = I | C
        .label("start")
            .BRK()
        .label("return_to")
            .NOP()
            .NOP()
        .org(0x3456)
        .label("interrupt")
            .NOP()
            .NOP()
        .label("return_from")
            .RTI()
        .org(0xfffc)                // RESET VECTOR
        .word("init")
        .org(0xFFFE)                // INTERRUPT VECTOR
        .word("interrupt")
        .compileTo(sram);

    // capture information from the assembler
    cpu6502::assembler::Address start("start");
    assembler.lookupAddress(start);

    cpu6502::assembler::Address interrupt("interrupt");
    assembler.lookupAddress(interrupt);

    cpu6502::assembler::Address returnTo("return_to");
    assembler.lookupAddress(returnTo);

    cpu6502::assembler::Address returnFrom("return_from");
    assembler.lookupAddress(returnFrom);

    // skip RESET
    helperSkipResetVector();

    // skip init section
    testBench.tick(4);

    // skip BRK
    testBench.tick(7);

    // capture SP after BRK
    const uint8_t sp = core.o_debug_s;

    // skip 2 x NOPs in 'interrupt' section
    testBench.tick(4);

    // verify that PC is where we expect
    EXPECT_EQ(returnFrom.lo(), core.o_debug_pcl);
    EXPECT_EQ(returnFrom.hi(), core.o_debug_pch);

    // simulate RTI and NOP 
    testBench.trace.clear();
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(8)
        .port(o_rw).signal("11").repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_address)
            .signal({
                // RTI - at 'return_from'
                returnFrom.byteIndex(),
                returnFrom.byteIndex() + 1u,
                0x0100u + sp,
                0x0100u + sp + 1u,
                0x0100u + sp + 2u,
                0x0100u + sp + 3u,

                // NOP - at 'return_to'
                returnTo.byteIndex(),
                returnTo.byteIndex() + 1u
            }).repeatEachStep(2)
        .port(o_debug_s)
            .signal({sp}).repeat(5)
            .signal({returnTo.lo()})        // todo: shouldn't be sensitive to internal workings
            .signal({sp + 3u}).repeat(2)
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    // processor flags should be restored
    EXPECT_EQ(I | C, core.o_debug_p);
}
