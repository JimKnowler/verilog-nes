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
    sram.clear(0);
    
    const uint8_t kTestData = 0xA4;
    sram.write(0x01FC, kTestData);

    Assembler()
        .PLA()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // simulate PLA and NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("1").repeat(10)
        .port(o_sync).signal("01001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 0x01FC, 1, 2})
                        .repeatEachStep(2)
        .port(o_debug_s).signal({0xFC}).repeat(4)
                        .signal({0xFD}).repeat(1)
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(3)
                         .signal({kTestData}).repeat(2)
                         .concat().repeatEachStep(2)
        .port(o_debug_x).signal({0xFF}).repeat(10)
        .port(o_debug_y).signal({0xFF}).repeat(10)
        .port(o_debug_p).signal({0x00}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementPLP) {
    sram.clear(0);
    
    const uint8_t kTestData = 0xFF;
    const uint8_t kExpectedP = C|Z|I|B|D|V|N;
    sram.write(0x01FC, kTestData);

    Assembler()
        .PLP()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // simulate PLP and NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("1").repeat(10)
        .port(o_sync).signal("01001").repeatEachStep(2)
        .port(o_address).signal({0, 1, 0x01FC, 1, 2})
                        .repeatEachStep(2)
        .port(o_debug_s).signal({0xFC}).repeat(4)
                        .signal({0xFD}).repeat(1)
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(10)
        .port(o_debug_x).signal({0xFF}).repeat(10)
        .port(o_debug_y).signal({0xFF}).repeat(10)
        .port(o_debug_p).signal({0x00}).repeat(3)
                        .signal({kExpectedP}).repeat(2)
                        .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
