#include "Cpu6502.fixture.h"

TEST_F(Cpu6502, ShouldImplementTAX) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x75;

    Assembler()
        .LDA().immediate(kTestData)
        .TAX()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate TAX and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData})
                        .repeat(8)
        .port(o_debug_x).signal({0xFF}).repeat(4)
                        .signal({kTestData}).repeat(4)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTAY) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x75;

    Assembler()
        .LDA().immediate(kTestData)
        .TAY()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate TAY and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData})
                        .repeat(8)
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(4)
                        .signal({kTestData}).repeat(4);
        
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTXA) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x73;

    Assembler()
        .LDX().immediate(kTestData)
        .TXA()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDX
    testBench.tick(2);
    testBench.trace.clear();

    // simulate TXA and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(4)
                        .signal({kTestData}).repeat(4)
        .port(o_debug_x).signal({kTestData})
                        .repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTYA) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x92;

    Assembler()
        .LDY().immediate(kTestData)
        .TYA()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDY
    testBench.tick(2);
    testBench.trace.clear();

    // simulate TYA and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(4)
                        .signal({kTestData}).repeat(4)
        .port(o_debug_x).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({kTestData})
                        .repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTXS) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x22;

    Assembler()
        .LDX().immediate(kTestData)
        .TXS()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDX
    testBench.tick(2);
    testBench.trace.clear();

    // simulate TXS and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(8)
        .port(o_debug_x).signal({kTestData})
                        .repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8)
        .port(o_debug_s).signal({0xFD}).repeat(4)
                        .signal({kTestData}).repeat(4);
        

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementTSX) {
sram.clear(0);
    
    const uint8_t kTestData = 0x22;

    Assembler()
        .LDX().immediate(kTestData)
        .TXS()
        .LDX().immediate(kTestData+1)
        .TSX()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDX, TXS, LDX
    testBench.tick(6);
    testBench.trace.clear();

    // simulate TSX and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({5, 6, 6, 7})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8)
        .port(o_debug_x).signal({kTestData+1}).repeat(4)
                        .signal({kTestData}).repeat(4)
        .port(o_debug_s).signal({kTestData})
                        .repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
