#include "Cpu6502.fixture.h"

TEST_F(Cpu6502, ShouldImplementBCCWhenCarrySet) {
    sram.clear(0);

    // BCC should not branch while Carry is set
    // BCC should finish in two cycles and excute NOP
    //     at PC=0x0003

    Assembler()
            .SEC()
            .BCC().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .label("skip_nop")
            .NOP()    
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("010101").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(12)
        .port(o_debug_x).signal({0xFF}).repeat(12)
        .port(o_debug_y).signal({0xFF}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBCCWhenCarryClear) {
    sram.clear(0);

    // BCC should branch while Carry is clear
    // BCC should finish in three cycles and branch to
    //     NOP at PC=0x0006

    Assembler()
            .CLC()
            .BCC().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .label("skip_nop")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("0101001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 6, 6, 7})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(14)
        .port(o_debug_x).signal({0xFF}).repeat(14)
        .port(o_debug_y).signal({0xFF}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBCCWhenCarryClearAndNewPage) {
     sram.clear(0);

    // BCC should branch while Carry is clear
    // BCC should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler assembler;
    assembler
            .NOP()
        .org(0x100 - (1 + 2 + 1 + 1 + 1))
        .label("start")
            .CLC()
            .BCC().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .label("skip_nop")
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    ::cpu6502::assembler::Address skip_nop("skip_nop");
    assembler.lookupAddress(skip_nop);
    ASSERT_EQ(0x100, skip_nop.byteIndex());

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("01010001").repeatEachStep(2)
        .port(o_address).signal({0xFA, 0xFB, 0xFB, 0xFC, 0, 0x100, 0x100, 0x101})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(16)
        .port(o_debug_x).signal({0xFF}).repeat(16)
        .port(o_debug_y).signal({0xFF}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
