#include "Cpu6502.fixture.hpp"

TEST_F(Cpu6502, ShouldImplementJSR) {
    sram.clear(0);

    // NOTE: using .org to locate this code in the middle of memory, so that PC
    //       has an interesting value with non-zero hi and lo bytes
    Assembler assembler;
    assembler
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
        .org(0xFFFC)
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
        .port(o_sync).signal("1010000010").repeatEachStep(2)
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
                                // NOP
                                jmpTo.byteIndex(),
                                jmpTo.byteIndex() + 1u
                                })
                        .repeatEachStep(2)
        .port(o_debug_s).signal({sp}).repeat(4)         
                        .signal({jmpTo.lo()}).repeat(4) // TODO: we're not worried about value of SP during execution
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

TEST_F(Cpu6502, ShouldImplementRTS) {
    sram.clear(0);

    // NOTE: using .org to locate this code in the middle of memory, so that PC
    //       has an interesting value with non-zero hi and lo bytes
    Assembler assembler;
    assembler
        .byte(0)
        .org(0x3456)
        .label("start")
            .NOP()
            .RTS()
            .NOP()
            .NOP()
            .NOP()
        .org(0x4567)
        .label("return_to")
            .NOP()
        .org(0xFFFC)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    const uint8_t sp = testBench.core().o_debug_s;

    cpu6502::assembler::Address start("start");
    assembler.lookupAddress(start);

    cpu6502::assembler::Address returnTo("return_to");
    assembler.lookupAddress(returnTo);

    // write returnTo-1 to stack, so that RTS will pick it up
    cpu6502::assembler::Address returnToMinus1(returnTo.byteIndex() - 1u);
    sram.write( 0x0100 + sp + 1, { returnToMinus1.lo(), returnToMinus1.hi() } );

    testBench.tick(10);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(10)
        .port(o_rw).signal("1111111111").repeatEachStep(2)
        .port(o_sync).signal("1010000010").repeatEachStep(2)
        .port(o_address).signal({
                                // NOP
                                start.byteIndex(),
                                start.byteIndex() + 1u,
                                // RTS
                                start.byteIndex() + 1u,     // T0
                                start.byteIndex() + 2u,     // T1
                                0x0100u + sp,               // T2
                                0x0100u + sp + 1u,          // T3
                                0x0100u + sp + 2u,          // T4
                                returnTo.byteIndex() - 1u,  // T5
                                // NOP
                                returnTo.byteIndex(),
                                returnTo.byteIndex() + 1u
                                })
                        .repeatEachStep(2)
        .port(o_debug_s).signal({sp}).repeat(6)
                        .signal({returnToMinus1.lo()})  // TODO: not interested in temporary changes to SP
                        .signal({sp + 2u}).repeat(3)    // TODO: but we are interested in what it ends with
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(20)
        .port(o_debug_x).signal({0xFF}).repeat(20)
        .port(o_debug_y).signal({0xFF}).repeat(20);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementJSRandRTS) {
    sram.clear(0);

    const uint8_t kTestValue = 0xAB;

    // NOTE: using .org to locate this code in the middle of memory, so that PC
    //       has an interesting value with non-zero hi and lo bytes
    Assembler assembler;
    assembler
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
            .LDA().immediate(kTestValue)
            .RTS()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(19);

    cpu6502::assembler::Address returnTo("return_to");
    assembler.lookupAddress(returnTo);

    auto& core = testBench.core();
    EXPECT_EQ(returnTo.byteIndex(), core.o_address);
    EXPECT_EQ(kTestValue, core.o_debug_ac);
}
