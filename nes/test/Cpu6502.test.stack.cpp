#include "Cpu6502.fixture.hpp"

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
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 0x01FC, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_s).signal({0xFC}).repeat(3)
                        .signal({0xFB}).repeat(2)
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(10)
        .port(o_debug_x).signal({0xFF}).repeat(10)
        .port(o_debug_y).signal({0xFF}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    EXPECT_EQ(kTestData, sram.read(0x01FC));
}

TEST_F(Cpu6502, ShouldImplementPHP) {
    sram.clear(0);
    
    const uint8_t kTestData = C|V|Z|N;

    Assembler()
        // load kTestData into P
        .LDA().immediate(kTestData)
        .PHA()
        .PLP()
        // test PHP
        .PHP()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(9);
    testBench.trace.clear();

    // simulate PHP and NOP
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11011").repeatEachStep(2)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({4, 5, 0x01FC, 5, 6})
                        .repeatEachStep(2)
        .port(o_debug_s).signal({0xFC}).repeat(3)
                        .signal({0xFB}).repeat(2)
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(10)
        .port(o_debug_x).signal({0xFF}).repeat(10)
        .port(o_debug_y).signal({0xFF}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    EXPECT_EQ(kTestData, sram.read(0x01FC));
}

TEST_F(Cpu6502, ShouldImplementPLA) {
    sram.clear(0);
    
    const uint8_t kTestData = 0xA4;
    sram.write(0x01FD, kTestData);

    Assembler()
        .PLA()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // simulate PLA and NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("1").repeat(12)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 0x01FC, 0x01FD, 1, 2})
                        .repeatEachStep(2)
        .port(o_debug_s).signal({0xFC}).repeat(3)
                        .signal({0xFD}).repeat(3)
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(4)
                         .signal({kTestData}).repeat(2)
                         .concat().repeatEachStep(2)
        .port(o_debug_x).signal({0xFF}).repeat(12)
        .port(o_debug_y).signal({0xFF}).repeat(12)
        .port(o_debug_p).signal({0x00}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementPLP) {
    sram.clear(0);
    
    const uint8_t kTestData = 0xFF;
    const uint8_t kExpectedP = C|Z|I|B|D|V|N;
    sram.write(0x01FD, kTestData);

    Assembler()
        .PLP()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // simulate PLP and NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("1").repeat(12)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 0x01FC, 0x1FD, 1, 2})
                        .repeatEachStep(2)
        .port(o_debug_s).signal({0xFC}).repeat(3)
                        .signal({0xFD}).repeat(3)
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(12)
        .port(o_debug_x).signal({0xFF}).repeat(12)
        .port(o_debug_y).signal({0xFF}).repeat(12)
        .port(o_debug_p).signal({0x00}).repeat(4)
                        .signal({kExpectedP}).repeat(2)
                        .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
