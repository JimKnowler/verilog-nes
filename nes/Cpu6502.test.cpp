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
    sram.clear(OPCODE_NOP);

    // write address for reset vector
    sram.write(0xFFFC, 0x02);               // low byte of 16bit address
    sram.write(0xFFFD, 0x80);               // high byte of 16bit address

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(9)
        .port(o_rw).signal("11").repeat(9)
        .port(o_sync).signal("010000001").repeatEachStep(2)
        .port(o_address)
            .signal({
                0x0000, 0x0001,             // PC, PC + 1
                0x01FF, 0x01FE, 0x01FD,     // SP, SP-1, SP-2
                0xFFFC, 0xFFFD,             // Reset Vector (low byte), Reset Vector (high byte)
                0x8002,                     // The reset vector (pointing at NOP)
                0x8003,                     // NOP incrememting PC
            }).repeatEachStep(2);
        /*
        // TODO: would need to be zipped with random data during phi2
        .port(i_data)
            .signal({0})                    // memory simulation is warming up
            .signal({OPCODE_NOP}).repeat(5)        // reading unset memory
            .signal({0x02, 0x80})           // load PC from RESET vector
            .signal({OPCODE_NOP})                  // load NOP instruction at RESET vector
            .concat().repeatEachStep(2);
        */

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

/// @note this test case is based on Ben Eater's video 
///       "'hello world' from scratch on a 6502 - Part 1"
///       https://www.youtube.com/watch?v=LnzuMJLZRdU
TEST_F(Cpu6502, ShouldImplementNOP) {
    sram.clear(OPCODE_NOP);

    helperSkipResetVector();

    testBench.tick(5);
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(5)
        .port(o_rw).signal("11").repeat(5)
        .port(o_sync).signal("01010").repeatEachStep(2)
        .port(o_address).signal({
            0xEAEA, 0xEAEB,     // NOP: T0, T1
            0xEAEB, 0xEAEC,     // NOP: T0, T1
            0xEAEC}             // NOP: T0
        ).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}
