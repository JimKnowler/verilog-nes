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

TEST_F(Cpu6502, ShouldImplementBMIWhenNegativeFlagClear) {
    sram.clear(0);

    // BMI should not branch while Negative Flag is clear
    // BMI should finish in two cycles and excute NOP
    //     at PC=0x0004

    Assembler()
            .LDA().immediate(0x7F)
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
                         .signal({0x7F}).repeat(8)
        .port(o_debug_x).signal({0xFF}).repeat(12)
        .port(o_debug_y).signal({0xFF}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBMIWhenNegativeFlagSet) {
    sram.clear(0);

    // BMI should branch while negative flag is set
    // BMI should finish in three cycles and branch to
    //     NOP at PC=0x0007

    Assembler()
            .LDA().immediate(0x80)
            .BMI().relative("skip_nop")
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
                         .signal({0x80}).repeat(10)
        .port(o_debug_x).signal({0xFF}).repeat(14)
        .port(o_debug_y).signal({0xFF}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBMIWhenNegativeFlagSetAndNewPage) {
     sram.clear(0);

    // BMI should branch while negative flag is set
    // BMI should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
        .label("start")
            .LDA().immediate(0x80)
            .BMI().relative("skip_nop")
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
                         .signal({0x80}).repeat(12)
        .port(o_debug_x).signal({0xFF}).repeat(16)
        .port(o_debug_y).signal({0xFF}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBPLWhenNegativeFlagSet) {
    sram.clear(0);

    // BPL should not branch while Negative Flag is set
    // BPL should finish in two cycles and excute NOP
    //     at PC=0x0004

    Assembler()
            .LDA().immediate(0x80)
            .BPL().relative("skip_nop")
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
                         .signal({0x80}).repeat(8)
        .port(o_debug_x).signal({0xFF}).repeat(12)
        .port(o_debug_y).signal({0xFF}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBPLWhenNegativeFlagClear) {
    sram.clear(0);

    // BPL should branch while negative flag is clear
    // BPL should finish in three cycles and branch to
    //     NOP at PC=0x0007

    Assembler()
            .LDA().immediate(0x7F)
            .BPL().relative("skip_nop")
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
                         .signal({0x7F}).repeat(10)
        .port(o_debug_x).signal({0xFF}).repeat(14)
        .port(o_debug_y).signal({0xFF}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBPLWhenNegativeFlagClearAndNewPage) {
     sram.clear(0);

    // BPL should branch while negative flag is clear
    // BPL should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
        .label("start")
            .LDA().immediate(0x7F)
            .BPL().relative("skip_nop")
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
                         .signal({0x7F}).repeat(12)
        .port(o_debug_x).signal({0xFF}).repeat(16)
        .port(o_debug_y).signal({0xFF}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBVSWhenOverflowFlagClear) {
    sram.clear(0);

    // BVS should not branch while Overflow Flag is clear
    // BVS should finish in two cycles and excute NOP
    //     at PC=0x0003

    Assembler()
            .CLV()
            .BVS().relative("skip_nop")
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

TEST_F(Cpu6502, ShouldImplementBVSWhenOverflowFlagSet) {
    sram.clear(0);

    // BVS should branch while overflow flag is set
    // BVS should finish in three cycles and branch to
    //     NOP at skip_nop

    Assembler assembler;
    assembler
            .BIT().absolute("overflow_flag")
            .BVS().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .label("skip_nop")
            .NOP()
        .label("overflow_flag")
            .byte(0x40)
        .compileTo(sram);

    cpu6502::assembler::Address overflowFlag("overflow_flag");
    assembler.lookupAddress(overflowFlag);

    cpu6502::assembler::Address skipNop("skip_nop");
    assembler.lookupAddress(skipNop);

    helperSkipResetVector();

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(9)
        .port(o_rw).signal("11")
                    .repeat(9)
        .port(o_sync).signal("010001001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, 
                                overflowFlag.byteIndex(), 
                                3, 4, 
                                skipNop.byteIndex(),
                                skipNop.byteIndex(),
                                uint32_t(skipNop.byteIndex()) + 1})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(18)
        .port(o_debug_x).signal({0xFF}).repeat(18)
        .port(o_debug_y).signal({0xFF}).repeat(18);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBVSWhenOverflowFlagSetAndNewPage) {
     sram.clear(0);

    // BVS should branch while overflow flag is set
    // BVS should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler assembler;
    assembler
        .label("start")
            .BIT().absolute("overflow_flag")
            .BVS().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("skip_nop")
            .NOP()
        .label("overflow_flag")
            .byte(0x40)
        .compileTo(sram);
    
    cpu6502::assembler::Address overflowFlag("overflow_flag");
    assembler.lookupAddress(overflowFlag);

    helperSkipResetVector();

    testBench.tick(10);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(10)
        .port(o_rw).signal("11")
                    .repeat(10)
        .port(o_sync).signal("0100010001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, overflowFlag.byteIndex(), 3, 4, 0, 0x100, 0x100, 0x101})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(20)
        .port(o_debug_x).signal({0xFF}).repeat(20)
        .port(o_debug_y).signal({0xFF}).repeat(20);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBVCWhenOverflowFlagSet) {
    sram.clear(0);

    // BVC should not branch while Overflow Flag is set
    // BVC should finish in two cycles and excute NOP
    //     at PC=0x0005

    Assembler assembler;
    assembler
            .BIT().absolute("overflow_flag")
            .BVC().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .label("skip_nop")
            .NOP()    
        .label("overflow_flag")
            .byte(0x40)
        .compileTo(sram);

    cpu6502::assembler::Address overflowFlag("overflow_flag");
    assembler.lookupAddress(overflowFlag);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("01000101").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, overflowFlag.byteIndex(), 3, 4, 5, 6})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(16)
        .port(o_debug_x).signal({0xFF}).repeat(16)
        .port(o_debug_y).signal({0xFF}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBVCWhenOverflowFlagClear) {
    sram.clear(0);

    // BCC should branch while overflow flag is clear
    // BCC should finish in three cycles and branch to
    //     NOP at skip_nop

    Assembler assembler;
    assembler
            .CLV()
            .BVC().relative("skip_nop")
            .NOP()
            .NOP()
            .NOP()
        .label("skip_nop")
            .NOP()
        .compileTo(sram);

    cpu6502::assembler::Address skipNop("skip_nop");
    assembler.lookupAddress(skipNop);

    helperSkipResetVector();

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("0101001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2,
                                skipNop.byteIndex(),
                                skipNop.byteIndex(),
                                uint32_t(skipNop.byteIndex()) + 1})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(14)
        .port(o_debug_x).signal({0xFF}).repeat(14)
        .port(o_debug_y).signal({0xFF}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBVCWhenOverflowFlagClearAndNewPage) {
    sram.clear(0);

    // BVC should branch while overflow flag is clear
    // BVC should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
        .label("start")
            .CLV()
            .BVC().relative("skip_nop")
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

TEST_F(Cpu6502, ShouldImplementJMPabsolute) {
    sram.clear(0);

    Assembler assembler;
    assembler
        .label("start")
            .NOP()
            .JMP().absolute("jmp_to")
            .NOP()
            .NOP()
            .NOP()
        .label("jmp_to")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address jmpTo("jmp_to");
    assembler.lookupAddress(jmpTo);

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("0101001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 3,
                                jmpTo.byteIndex(),
                                uint32_t(jmpTo.byteIndex()) + 1})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(14)
        .port(o_debug_x).signal({0xFF}).repeat(14)
        .port(o_debug_y).signal({0xFF}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

