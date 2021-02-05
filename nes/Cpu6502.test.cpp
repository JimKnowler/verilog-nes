#include "Cpu6502.fixture.h"

TEST_F(Cpu6502, ShouldConstruct) {
}

TEST_F(Cpu6502, ShouldReset) {
    testBench.reset();

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
        .port(o_address).signal({0,0})
        .port(o_data).signal({0,0});

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementResetVector) {
    sram.clear(NOP());

    // write address for reset vector
    sram.write(0xFFFC, 0x02);               // low byte of 16bit address
    sram.write(0xFFFD, 0x80);               // high byte of 16bit address

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(9)
        .port(o_rw).signal("11").repeat(9)
        .port(o_sync).signal("100000010").repeatEachStep(2)
        .port(o_address)
            .signal({
                0x0000, 0x0001,             // PC, PC + 1
                0x01FF, 0x01FE, 0x01FD,     // SP, SP-1, SP-2
                0xFFFC, 0xFFFD,             // Reset Vector (low byte), Reset Vector (high byte)
                0x8002,                     // The reset vector (pointing at NOP)
                0x8003,                     // NOP incrememting PC
            }).repeatEachStep(2)
        .port(o_debug_s)
            .signal({0xFF}).repeat(6)
            .signal({0xFC}).repeat(3)       // store SP-3 after 3rd push
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementNOP) {
    sram.clear(NOP());

    helperSkipResetVector();

    testBench.tick(5);
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(5)
        .port(o_rw).signal("11").repeat(5)
        .port(o_sync).signal("10101").repeatEachStep(2)
        .port(o_address).signal({
            0xEAEA, 0xEAEB,     // NOP: T0, T1
            0xEAEB, 0xEAEC,     // NOP: T0, T1
            0xEAEC}             // NOP: T0
        ).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
