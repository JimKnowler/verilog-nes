#include "Cpu6502.fixture.h"

TEST_F(Cpu6502, ShouldImplementPHA) {
    sram.clear(0);
    
    const uint8_t kTestData = 0xA4;

    Assembler()
        .LDA().immediate(kTestData)
        .PHA()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate PHA and NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11011").repeatEachStep(2)
        .port(o_sync).signal("01001").repeatEachStep(2)
        .port(o_address).signal({2, 3, 0x01FC, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_s).signal({0xFC}).repeat(4)
                        .signal({0xFB}).repeat(1)
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({0xA4}).repeat(10)
        .port(o_debug_x).signal({0xFF}).repeat(10)
        .port(o_debug_y).signal({0xFF}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    EXPECT_EQ(kTestData, sram.read(0x01FC));
}

TEST_F(Cpu6502, ShouldImplementPHP) {
    // NOTE: implement PLP first, so that we can load P with known data
}

TEST_F(Cpu6502, ShouldImplementPLA) {
    
}

TEST_F(Cpu6502, ShouldImplementPLP) {
    
}
