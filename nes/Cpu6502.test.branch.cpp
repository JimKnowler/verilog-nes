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

    Assembler()
        .label("start")
            .CLC()
            .BCC().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("skip_nop")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("01010001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 0, 0x100, 0x100, 0x101})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(16)
        .port(o_debug_x).signal({0xFF}).repeat(16)
        .port(o_debug_y).signal({0xFF}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBCSWhenCarryClear) {
    sram.clear(0);

    // BCS should not branch while Carry is clear
    // BCS should finish in two cycles and excute NOP
    //     at PC=0x0003

    Assembler()
            .CLC()
            .BCS().relative("skip_nop")
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

TEST_F(Cpu6502, ShouldImplementBCSWhenCarrySet) {
    sram.clear(0);

    // BCS should branch while Carry is set
    // BCS should finish in three cycles and branch to
    //     NOP at PC=0x0006

    Assembler()
            .SEC()
            .BCS().relative("skip_nop")
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

TEST_F(Cpu6502, ShouldImplementBCSWhenCarrySetAndNewPage) {
     sram.clear(0);

    // BCS should branch while Carry is set
    // BCS should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
        .label("start")
            .SEC()
            .BCS().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("skip_nop")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("01010001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 0, 0x100, 0x100, 0x101})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(16)
        .port(o_debug_x).signal({0xFF}).repeat(16)
        .port(o_debug_y).signal({0xFF}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBEQWhenZeroFlagClear) {
    sram.clear(0);

    // BEQ should not branch while Zero Flag is clear
    // BEQ should finish in two cycles and excute NOP
    //     at PC=0x0004

    Assembler()
            .LDA().immediate(1)
            .BCS().relative("skip_nop")
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
        .port(o_address).signal({0, 1, 2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(4)
                         .signal({0x01}).repeat(8)
        .port(o_debug_x).signal({0xFF}).repeat(12)
        .port(o_debug_y).signal({0xFF}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBEQWhenZeroFlagSet) {
    sram.clear(0);

    // BEQ should branch while Carry is set
    // BEQ should finish in three cycles and branch to
    //     NOP at PC=0x0007

    Assembler()
            .LDA().immediate(0x00)
            .BEQ().relative("skip_nop")
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
        .port(o_address).signal({0, 1, 2, 3, 7, 7, 8})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(4)
                         .signal({0x00}).repeat(10)
        .port(o_debug_x).signal({0xFF}).repeat(14)
        .port(o_debug_y).signal({0xFF}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBEQWhenZeroFlagSetAndNewPage) {
     sram.clear(0);

    // BEQ should branch while Carry is set
    // BEQ should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
        .label("start")
            .LDA().immediate(0x00)
            .BEQ().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("skip_nop")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("01010001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, 3, 0, 0x100, 0x100, 0x101})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(4)
                         .signal({0x00}).repeat(12)
        .port(o_debug_x).signal({0xFF}).repeat(16)
        .port(o_debug_y).signal({0xFF}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBNEWhenZeroFlagSet) {
    sram.clear(0);

    // BNE should not branch while Zero Flag is set
    // BNE should finish in two cycles and excute NOP
    //     at PC=0x0004

    Assembler()
            .LDA().immediate(0)
            .BNE().relative("skip_nop")
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
        .port(o_address).signal({0, 1, 2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(4)
                         .signal({0x00}).repeat(8)
        .port(o_debug_x).signal({0xFF}).repeat(12)
        .port(o_debug_y).signal({0xFF}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBNEWhenZeroFlagClear) {
    sram.clear(0);

    // BNE should branch while Zero flag is clear
    // BNE should finish in three cycles and branch to
    //     NOP at PC=0x0007

    Assembler()
            .LDA().immediate(0x01)
            .BNE().relative("skip_nop")
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
        .port(o_address).signal({0, 1, 2, 3, 7, 7, 8})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(4)
                         .signal({0x01}).repeat(10)
        .port(o_debug_x).signal({0xFF}).repeat(14)
        .port(o_debug_y).signal({0xFF}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBNEWhenZeroFlagClearAndNewPage) {
     sram.clear(0);

    // BNE should branch while Zero is clear
    // BNE should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
        .label("start")
            .LDA().immediate(0x01)
            .BNE().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("skip_nop")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("01010001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, 3, 0, 0x100, 0x100, 0x101})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(4)
                         .signal({0x01}).repeat(12)
        .port(o_debug_x).signal({0xFF}).repeat(16)
        .port(o_debug_y).signal({0xFF}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}