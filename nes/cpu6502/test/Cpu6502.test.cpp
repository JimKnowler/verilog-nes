#include "Cpu6502.fixture.hpp"

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
    sram.write(0xFFFC, 0x12);               // low byte of 16bit address
    sram.write(0xFFFD, 0x80);               // high byte of 16bit address

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(9)
        .port(o_rw).signal("11").repeat(9)
        .port(o_sync).signal("100000010").repeatEachStep(2)
        .port(o_address)
            .signal({
                0x0000, 0x0000,             // PC, PC
                0x0100, 0x01FF, 0x01FE,     // SP, SP-1, SP-2
                0xFFFC, 0xFFFD,             // Reset Vector (low byte), Reset Vector (high byte)
                0x8012,                     // The reset vector (pointing at NOP)
                0x8013,                     // NOP incrememting PC
            }).repeatEachStep(2)
        .port(o_debug_p)
            .signal({U}).repeat(7)
            .signal({U|I}).repeat(2)        // based on feedback from nestest - I is set during RESET vector
            .concat().repeatEachStep(2)
        .port(o_debug_s)
            .signal({0x00}).repeat(5)
            .signal({0xFD}).repeat(4)       // store SP-3 after 3rd push
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementResetVectorWhileIRQ) {
    sram.clear(NOP());

    testBench.core().i_irq_n = 0;

    // write address for reset vector
    sram.write(0xFFFC, 0x12);               // low byte of 16bit address
    sram.write(0xFFFD, 0x80);               // high byte of 16bit address

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(9)
        .port(o_rw).signal("11").repeat(9)
        .port(o_sync).signal("100000010").repeatEachStep(2)
        .port(o_address)
            .signal({
                0x0000, 0x0000,             // PC, PC
                0x0100, 0x01FF, 0x01FE,     // SP, SP-1, SP-2
                0xFFFC, 0xFFFD,             // Reset Vector (low byte), Reset Vector (high byte)
                0x8012,                     // The reset vector (pointing at NOP)
                0x8013,                     // NOP incrememting PC
            }).repeatEachStep(2)
        .port(o_debug_p)
            .signal({U}).repeat(7)
            .signal({U|I}).repeat(2)        // based on feedback from nestest - I is set during RESET vector
            .concat().repeatEachStep(2)
        .port(o_debug_s)
            .signal({0x00}).repeat(5)
            .signal({0xFD}).repeat(4)       // store SP-3 after 3rd push
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementResetVectorWhileNMI) {
    sram.clear(NOP());

    // write address for reset vector
    sram.write(0xFFFC, 0x12);               // low byte of 16bit address
    sram.write(0xFFFD, 0x80);               // high byte of 16bit address

    testBench.tick(1);
    testBench.core().i_nmi_n = 0;
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(9)
        .port(o_rw).signal("11").repeat(9)
        .port(o_sync).signal("100000010").repeatEachStep(2)
        .port(o_address)
            .signal({
                0x0000, 0x0000,             // PC, PC
                0x0100, 0x01FF, 0x01FE,     // SP, SP-1, SP-2
                0xFFFC, 0xFFFD,             // Reset Vector (low byte), Reset Vector (high byte)
                0x8012,                     // The reset vector (pointing at NOP)
                0x8013,                     // NOP incrememting PC
            }).repeatEachStep(2)
        .port(o_debug_p)
            .signal({U}).repeat(7)
            .signal({U|I}).repeat(2)        // based on feedback from nestest - I is set during RESET vector
            .concat().repeatEachStep(2)
        .port(o_debug_s)
            .signal({0x00}).repeat(5)
            .signal({0xFD}).repeat(4)       // store SP-3 after 3rd push
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

TEST_F(Cpu6502, ShouldIncrementProgramCounter) {
    sram.clear(NOP());

    // reset vector
    sram.write(0xfffc, 0);
    sram.write(0xfffd, 0);

    helperSkipResetVector();

    const int kNumClockTicks = 4096;
    const int kNumClockTicksPerNOP = 2;

    testBench.tick(kNumClockTicks);

    const uint16_t kExpectedAddress = (kNumClockTicks/kNumClockTicksPerNOP);

    EXPECT_EQ(helperGetPC(), kExpectedAddress);
    EXPECT_EQ(testBench.core().o_address, kExpectedAddress);   
}
