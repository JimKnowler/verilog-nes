#include "Cpu6502.fixture.hpp"

// BRANCH OPCODES
// note: There is a peculiarity documented here around how branch opcodes can mask nmi/irq in certain circumstances
//       due to skipping T0 of next opcode and going directly to T1
//       I have gone for retaining T0 of next opcode, so that SYNC pin works as expected for debugging

TEST_F(Cpu6502, ShouldImplementBCCWhenCarrySet) {
    sram.clear(0);

    // BCC should not branch while Carry is set
    // BCC should finish in two cycles and excute NOP
    //     at PC=0x0003

    Assembler()
            .SEC()
            .BCC().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()    
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("101010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(12)
        .port(o_debug_x).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({0x00}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBCCWhenCarryClear) {
    sram.clear(0);

    // BCC should branch while Carry is clear
    // BCC should finish in three cycles and branch to
    //     NOP at PC=0x0006

    Assembler()
            .CLC()
            .BCC().relative("target")
            .NOP()
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1010010").repeatEachStep(2)
        .port(o_address).signal({
                                // CLC
                                0, 
                                1, 
                                // BCC (branch taken)
                                1,      // PC
                                2,      // PC + 1
                                3,      // PC + 2
                                // NOP
                                7,      // PC + 2 + relative
                                8})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(14)
        .port(o_debug_x).signal({0x00}).repeat(14)
        .port(o_debug_y).signal({0x00}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBCCWhenCarryClearAndNextPage) {
     sram.clear(0);

    // BCC should branch while Carry is clear
    // BCC should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
            .NOP()
        .org(0x00f0)
        .label("start")
            .CLC()
            .BCC().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("target")
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10100010").repeatEachStep(2)
        .port(o_address).signal({
                            // CLC
                            0x00f0,
                            0x00f1,
                            // BCC
                            0x00f1,         // PC
                            0x00f2,         // PC + 1
                            0x00f3,         // PC + 2
                            0x0000,         // PC + offset (w/o carry)
                            // NOP 
                            0x0100,         // PC + offset (w/ carry)
                            0x0101
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(16)
        .port(o_debug_x).signal({0x00}).repeat(16)
        .port(o_debug_y).signal({0x00}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBCSWhenCarryClear) {
    sram.clear(0);

    // BCS should not branch while Carry is clear
    // BCS should finish in two cycles and excute NOP
    //     at PC=0x0003

    Assembler()
            .CLC()
            .BCS().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()    
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("101010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(12)
        .port(o_debug_x).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({0x00}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBCSWhenCarrySet) {
    sram.clear(0);

    // BCS should branch while Carry is set
    // BCS should finish in three cycles and branch to
    //     NOP at PC=0x0006

    Assembler()
            .SEC()
            .BCS().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1010010").repeatEachStep(2)
        .port(o_address).signal({
                        // SEC
                        0, 
                        1, 
                        // BCS (branch taken)
                        1,      // PC
                        2,      // PC + 1
                        3,      // PC + 2
                        // NOP
                        6,      // PC + 2 + relative
                        7})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(14)
        .port(o_debug_x).signal({0x00}).repeat(14)
        .port(o_debug_y).signal({0x00}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBCSWhenCarrySetAndNextPage) {
     sram.clear(0);

    // BCS should branch while Carry is set
    // BCS should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
            .NOP()
        .org(0x00f0)
        .label("start")
            .SEC()
            .BCS().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("target")
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10100010").repeatEachStep(2)
        .port(o_address).signal({
                            // SEC
                            0x00f0,
                            0x00f1,
                            // BCS
                            0x00f1,         // PC
                            0x00f2,         // PC + 1
                            0x00f3,         // PC + 2
                            0x0000,         // PC + offset (w/o carry)
                            // NOP 
                            0x0100,         // PC + offset (w/ carry)
                            0x0101
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(16)
        .port(o_debug_x).signal({0x00}).repeat(16)
        .port(o_debug_y).signal({0x00}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBEQWhenZeroFlagClear) {
    sram.clear(0);

    // BEQ should not branch while Zero Flag is clear
    // BEQ should finish in two cycles and excute NOP
    //     at PC=0x0004

    Assembler()
            .LDA().immediate(1)
            .BCS().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()    
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("101010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x01}).repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({0x00}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBEQWhenZeroFlagSet) {
    sram.clear(0);

    // BEQ should branch while Carry is set
    // BEQ should finish in three cycles and branch to
    //     NOP at PC=0x0007

    Assembler()
            .LDA().immediate(0x00)
            .BEQ().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1010010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDA
                            0, 
                            1, 
                            // BEQ (branch taken)
                            2,      // PC
                            3,      // PC + 1
                            4,      // PC + 2
                            // NOP
                            7,      // PC + 2 + relative
                            8})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x00}).repeat(10)
        .port(o_debug_x).signal({0x00}).repeat(14)
        .port(o_debug_y).signal({0x00}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBEQWhenZeroFlagSetAndNextPage) {
     sram.clear(0);

    // BEQ should branch while Carry is set
    // BEQ should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
            .NOP()
        .org(0x00f0)
        .label("start")
            .LDA().immediate(0x00)
            .BEQ().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("target")
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10100010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDA
                            0x00f0,
                            0x00f1,
                            // BEQ
                            0x00f2,         // PC
                            0x00f3,         // PC + 1
                            0x00f4,         // PC + 2
                            0x0000,         // PC + offset (w/o carry)
                            // NOP 
                            0x0100,         // PC + offset (w/ carry)
                            0x0101
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x00}).repeat(12)
        .port(o_debug_x).signal({0x00}).repeat(16)
        .port(o_debug_y).signal({0x00}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBNEWhenZeroFlagSet) {
    sram.clear(0);

    // BNE should not branch while Zero Flag is set
    // BNE should finish in two cycles and excute NOP
    //     at PC=0x0004

    Assembler()
            .LDA().immediate(0)
            .BNE().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()    
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("101010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x00}).repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({0x00}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBNEWhenZeroFlagClear) {
    sram.clear(0);

    // BNE should branch while Zero flag is clear
    // BNE should finish in three cycles and branch to
    //     NOP at PC=0x0007

    Assembler()
            .LDA().immediate(0x01)
            .BNE().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1010010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDA
                            0, 
                            1, 
                            // BNE (branch taken)
                            2,      // PC
                            3,      // PC + 1
                            4,      // PC + 2
                            // NOP
                            7,      // PC + 2 + relative
                            8})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x01}).repeat(10)
        .port(o_debug_x).signal({0x00}).repeat(14)
        .port(o_debug_y).signal({0x00}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBNEWhenZeroFlagClearAndNextPage) {
     sram.clear(0);

    // BNE should branch while Zero is clear
    // BNE should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
            .NOP()
        .org(0x00f0)
        .label("start")
            .LDA().immediate(0x01)
            .BNE().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("target")
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10100010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDA
                            0x00f0,
                            0x00f1,
                            // BNE
                            0x00f2,         // PC
                            0x00f3,         // PC + 1
                            0x00f4,         // PC + 2
                            0x0000,         // PC + offset (w/o carry)
                            // NOP 
                            0x0100,         // PC + offset (w/ carry)
                            0x0101
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x01}).repeat(12)
        .port(o_debug_x).signal({0x00}).repeat(16)
        .port(o_debug_y).signal({0x00}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBMIWhenNegativeFlagClear) {
    sram.clear(0);

    // BMI should not branch while Negative Flag is clear
    // BMI should finish in two cycles and excute NOP
    //     at PC=0x0004

    Assembler()
            .LDA().immediate(0x7F)
            .BCS().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()    
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("101010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x7F}).repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({0x00}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBMIWhenNegativeFlagSet) {
    sram.clear(0);

    // BMI should branch while negative flag is set
    // BMI should finish in three cycles and branch to
    //     NOP at PC=0x0007

    Assembler()
            .LDA().immediate(0x80)
            .BMI().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1010010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDA
                            0, 
                            1, 
                            // BMI (branch taken)
                            2,      // PC
                            3,      // PC + 1
                            4,      // PC + 2
                            // NOP
                            7,      // PC + 2 + relative
                            8})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x80}).repeat(10)
        .port(o_debug_x).signal({0x00}).repeat(14)
        .port(o_debug_y).signal({0x00}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBMIWhenNegativeFlagSetAndNextPage) {
     sram.clear(0);

    // BMI should branch while negative flag is set
    // BMI should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
            .NOP()
        .org(0x00f0)
        .label("start")
            .LDA().immediate(0x80)
            .BMI().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("target")
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10100010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDA
                            0x00f0,
                            0x00f1,
                            // BMI
                            0x00f2,         // PC
                            0x00f3,         // PC + 1
                            0x00f4,         // PC + 2
                            0x0000,         // PC + offset (w/o carry)
                            // NOP 
                            0x0100,         // PC + offset (w/ carry)
                            0x0101
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x80}).repeat(12)
        .port(o_debug_x).signal({0x00}).repeat(16)
        .port(o_debug_y).signal({0x00}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBPLWhenNegativeFlagSet) {
    sram.clear(0);

    // BPL should not branch while Negative Flag is set
    // BPL should finish in two cycles and execute NOP
    //     at PC=0x0004

    Assembler()
            .LDA().immediate(0x80)
            .BPL().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()    
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("101010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, 3, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x80}).repeat(8)
        .port(o_debug_x).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({0x00}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBPLWhenNegativeFlagClear) {
    sram.clear(0);

    // BPL should branch while negative flag is clear
    // BPL should finish in three cycles and branch to
    //     NOP at PC=0x0007

    Assembler()
            .LDA().immediate(0x7F)
            .BPL().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1010010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDA
                            0, 
                            1, 
                            // BPL (branch taken)
                            2,      // PC
                            3,      // PC + 1
                            4,      // PC + 2
                            // NOP
                            7,      // PC + 2 + relative
                            8})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x7F}).repeat(10)
        .port(o_debug_x).signal({0x00}).repeat(14)
        .port(o_debug_y).signal({0x00}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBPLWhenNegativeFlagClearAndNextPage) {
     sram.clear(0);

    // BPL should branch while negative flag is clear
    // BPL should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
            .NOP()
        .org(0x00f0)
        .label("start")
            .LDA().immediate(0x7F)
            .BPL().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("target")
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10100010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDA
                            0x00f0,
                            0x00f1,
                            // BPL
                            0x00f2,         // PC
                            0x00f3,         // PC + 1
                            0x00f4,         // PC + 2
                            0x0000,         // PC + offset (w/o carry)
                            // NOP 
                            0x0100,         // PC + offset (w/ carry)
                            0x0101
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x7F}).repeat(12)
        .port(o_debug_x).signal({0x00}).repeat(16)
        .port(o_debug_y).signal({0x00}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBPLWhenNegativeFlagClearAndBranchingBack) {
    sram.clear(0);

    // BPL should branch while negative flag is clear
    // BPL should finish in three cycles and branch back to
    //     NOP at PC=0x0000

    Assembler()
        .label("target")
            .NOP()
            .NOP()
        .label("start")
            .LDA().immediate(0x7F)
            .BPL().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1010010").repeatEachStep(2)
        .port(o_address).signal({
                                // LDA
                                2,
                                3,
                                // BPL
                                4,
                                5,
                                6,
                                // NOP (target)
                                0,
                                1
                                })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x7F}).repeat(10)
        .port(o_debug_x).signal({0x00}).repeat(14)
        .port(o_debug_y).signal({0x00}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBPLWhenNegativeFlagClearAndPreviousPage) {
     sram.clear(0);

    // BPL should branch while negative flag is clear
    // BPL should finish in four cycles and branch to
    //     NOP at PC=0x00f0

    Assembler()
            .NOP()
        .org(0x00f0)
        .label("target")
            .NOP()
        .org(0x0100)
        .label("start")
            .LDA().immediate(0x7F)
            .BPL().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10100010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDA
                            0x0100,
                            0x0101,
                            // BPL
                            0x0102,         // PC
                            0x0103,         // PC + 1
                            0x0104,         // PC + 2
                            0x01f0,         // PC + offset (w/o carry)
                            // NOP 
                            0x00f0,         // PC + offset (w/ carry)
                            0x00f1
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(4)
                         .signal({0x7F}).repeat(12)
        .port(o_debug_x).signal({0x00}).repeat(16)
        .port(o_debug_y).signal({0x00}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBVSWhenOverflowFlagClear) {
    sram.clear(0);

    // BVS should not branch while Overflow Flag is clear
    // BVS should finish in two cycles and excute NOP
    //     at PC=0x0003

    Assembler()
            .CLV()
            .BVS().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()    
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("101010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(12)
        .port(o_debug_x).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({0x00}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBVSWhenOverflowFlagSet) {
    sram.clear(0);

    // BVS should branch while overflow flag is set
    // BVS should finish in three cycles and branch to
    //     NOP at target

    Assembler assembler;
    assembler
            .BIT().absolute("overflow_flag")
            .BVS().relative("target")
            .NOP()
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()
        .label("overflow_flag")
            .byte(0x40)
        .compileTo(sram);

    cpu6502::assembler::Address overflowFlag("overflow_flag");
    assembler.lookupAddress(overflowFlag);

    cpu6502::assembler::Address target("target");
    assembler.lookupAddress(target);

    helperSkipResetVector();

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(9)
        .port(o_rw).signal("11")
                    .repeat(9)
        .port(o_sync).signal("100010010").repeatEachStep(2)
        .port(o_address).signal({
                                // BIT
                                0, 1, 2, 
                                overflowFlag.byteIndex(), 
                                // BVS
                                3,      // PC
                                4,      // PC + 1
                                5,      // PC + 2
                                // NOP
                                target.byteIndex(),
                                uint32_t(target.byteIndex()) + 1})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(18)
        .port(o_debug_x).signal({0x00}).repeat(18)
        .port(o_debug_y).signal({0x00}).repeat(18);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBVSWhenOverflowFlagSetAndNextPage) {
     sram.clear(0);

    // BVS should branch while overflow flag is set
    // BVS should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler assembler;
    assembler
            .NOP()
        .org(0x00f0)
        .label("start")
            .BIT().absolute("overflow_flag")
            .BVS().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("target")
            .NOP()
        .label("overflow_flag")
            .byte(0x40)
        .org(0xfffc)
        .word("start")
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
        .port(o_sync).signal("1000100010").repeatEachStep(2)
        .port(o_address).signal({
                            // BIT
                            0x00f0,
                            0x00f1,
                            0x00f2,
                            overflowFlag.byteIndex(),
                            // BVS
                            0x00f3,
                            0x00f4,
                            0x00f5,
                            0x0000,
                            // NOP (target)
                            0x0100,
                            0x0101
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(20)
        .port(o_debug_x).signal({0x00}).repeat(20)
        .port(o_debug_y).signal({0x00}).repeat(20);

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
            .BVC().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
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
        .port(o_sync).signal("10001010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, overflowFlag.byteIndex(), 3, 4, 5, 6})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(16)
        .port(o_debug_x).signal({0x00}).repeat(16)
        .port(o_debug_y).signal({0x00}).repeat(16);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBVCWhenOverflowFlagClear) {
    sram.clear(0);

    // BCC should branch while overflow flag is clear
    // BCC should finish in three cycles and branch to
    //     NOP at target

    Assembler assembler;
    assembler
            .CLV()
            .BVC().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .label("target")
            .NOP()
        .compileTo(sram);

    cpu6502::assembler::Address target("target");
    assembler.lookupAddress(target);

    helperSkipResetVector();

    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1010010").repeatEachStep(2)
        .port(o_address).signal({
                                // NOP
                                0, 1,
                                // BVC
                                1, 2, 3,
                                // NOP @ target
                                target.byteIndex(),
                                uint32_t(target.byteIndex()) + 1})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(14)
        .port(o_debug_x).signal({0x00}).repeat(14)
        .port(o_debug_y).signal({0x00}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementBVCWhenOverflowFlagClearAndNextPage) {
    sram.clear(0);

    // BVC should branch while overflow flag is clear
    // BVC should finish in four cycles and branch to
    //     NOP at PC=0x0100

    Assembler()
            .NOP()
        .org(0x00f0)
        .label("start")
            .CLV()
            .BVC().relative("target")
            .NOP()
            .NOP()
            .NOP()
        .org(0x0100)
        .label("target")
            .NOP()
        .org(0xfffc)
        .word("start")
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10100010").repeatEachStep(2)
        .port(o_address).signal({
                            // CLV
                            0x00f0,
                            0x00f1,
                            // BVC
                            0x00f1,
                            0x00f2,
                            0x00f3,
                            0x0000,
                            // NOP (target)
                            0x0100,
                            0x0101
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(16)
        .port(o_debug_x).signal({0x00}).repeat(16)
        .port(o_debug_y).signal({0x00}).repeat(16);

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
        .org(0x2345)
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
        .port(o_sync).signal("1010010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 3,
                                jmpTo.byteIndex(),
                                jmpTo.byteIndex() + 1u})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(14)
        .port(o_debug_x).signal({0x00}).repeat(14)
        .port(o_debug_y).signal({0x00}).repeat(14);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementJMPindirect) {
    sram.clear(0);

    Assembler assembler;
    assembler
        .label("start")
            .NOP()
            .JMP().indirect("jmp_address")
            .NOP()
            .NOP()
            .NOP()
        .org(0x1234)
        .label("jmp_to")
            .NOP()
        .org(0x4321)
        .label("jmp_address")
        .word("jmp_to")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address jmpAddress("jmp_address");
    assembler.lookupAddress(jmpAddress);

    cpu6502::assembler::Address jmpTo("jmp_to");
    assembler.lookupAddress(jmpTo);

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(9)
        .port(o_rw).signal("11")
                    .repeat(9)
        .port(o_sync).signal("101000010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 3,
                                jmpAddress.byteIndex(),
                                jmpAddress.byteIndex() + 1u,
                                jmpTo.byteIndex(),
                                jmpTo.byteIndex() + 1u})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(18)
        .port(o_debug_x).signal({0x00}).repeat(18)
        .port(o_debug_y).signal({0x00}).repeat(18);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementJMPindirectAtPageBoundary) {
    sram.clear(0);

    Assembler assembler;
    assembler
        .label("start")
            .NOP()
            .JMP().indirect(0x21FF)
            .NOP()
            .NOP()
            .NOP()
        .org(0x1234)
        .label("jmp_to")
            .NOP()
        .org(0x2100)
        .byte(0x12)         // MSB of target address
        .org(0x21FF)
        .byte(0x34)         // LSB of target address
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address jmpAddress("jmp_address");
    assembler.lookupAddress(jmpAddress);

    cpu6502::assembler::Address jmpTo("jmp_to");
    assembler.lookupAddress(jmpTo);

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(9)
        .port(o_rw).signal("11")
                    .repeat(9)
        .port(o_sync).signal("101000010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 1, 2, 3,
                                0x21FF,                 // LSB
                                0x2100,                 // MSB - original 6502 bug where address wraps around incorrectly
                                jmpTo.byteIndex(),
                                jmpTo.byteIndex() + 1u})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(18)
        .port(o_debug_x).signal({0x00}).repeat(18)
        .port(o_debug_y).signal({0x00}).repeat(18);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

// todo: negative relative offsets for branches BCC, BEQ, BNE, BMI, BVS, BVC (currently tested only for BPL)
