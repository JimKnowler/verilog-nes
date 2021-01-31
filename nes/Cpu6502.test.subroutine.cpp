#include "Cpu6502.fixture.h"

TEST_F(Cpu6502, ShouldImplementJSR) {
    sram.clear(0);

    // NOTE: using .org to locate this code in the middle of memory, so that PC
    //       has an interesting value with non-zero hi and lo bytes
    Assembler assembler;
    assembler
        .label("start")
            .NOP()
        .org(0x3456)
        .label("start")
            .NOP()
            .JSR().absolute("jmp_to")
        .label("return_to")
            .NOP()
            .NOP()
            .NOP()
        .org(0x4567)
        .label("jmp_to")
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    const uint8_t sp = testBench.core().o_debug_s;

    cpu6502::assembler::Address start("start");
    assembler.lookupAddress(start);

    cpu6502::assembler::Address jmpTo("jmp_to");
    assembler.lookupAddress(jmpTo);

    cpu6502::assembler::Address returnTo("return_to");
    assembler.lookupAddress(returnTo);

    testBench.tick(10);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(10)
        .port(o_rw).signal("1111100111").repeatEachStep(2)
        .port(o_sync).signal("0101000001").repeatEachStep(2)
        .port(o_address).signal({
                                // NOP
                                start.byteIndex(),
                                start.byteIndex() + 1u,
                                // JSR
                                start.byteIndex() + 1u,
                                start.byteIndex() + 2u,
                                0x0100u + sp,
                                0x0100u + sp,
                                0x0100u + sp - 1u,
                                start.byteIndex() + 3u,
                                jmpTo.byteIndex(),
                                jmpTo.byteIndex() + 1u
                                })
                        .repeatEachStep(2)
        .port(o_debug_s).signal({sp}).repeat(5)         
                        .signal({jmpTo.lo()}).repeat(3) // TODO: we're not worried about value of SP during execution
                        .signal({sp - 2u}).repeat(2)    // TODO: but we are interested in what it ends with
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(20)
        .port(o_debug_x).signal({0xFF}).repeat(20)
        .port(o_debug_y).signal({0xFF}).repeat(20);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    cpu6502::assembler::Address returnToMinus1(returnTo.byteIndex() - 1);
    EXPECT_EQ(returnToMinus1.hi(), sram.read(0x0100u + sp));
    EXPECT_EQ(returnToMinus1.lo(), sram.read(0x0100u + sp - 1u));
}
