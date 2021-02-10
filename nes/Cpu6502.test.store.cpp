#include "Cpu6502.fixture.hpp"

TEST_F(Cpu6502, ShouldImplementSTAa) {
    sram.clear(0);
    
    const uint16_t kTestAddress = 0x1234;
    const uint8_t kTestData = 0x42;

    Assembler()
        .LDA().immediate(kTestData)
        .STA().a(kTestAddress)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAi
    testBench.tick(2);
    testBench.trace.clear();

    // test STAa (then NOP)
    testBench.tick(6);

    // note: o_data has valid data for step before and after
    //       write data should be valid at end of phi 2
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_rw).signal("11").repeat(3)      // READ (STAa)
                    .signal("00")               // WRITE (STAa)
                    .signal("11").repeat(2)     // READ (NOP)
        .port(o_data).signal({0}).repeatEachStep(2).repeat(3)
                     .signal({0, kTestData, kTestData, 0})
                     .signal({0}).repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, kTestAddress})    // STAa
                        .signal({5, 6 })                    // NOP
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(6)
                         .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
    EXPECT_EQ(kTestData, sram.read(kTestAddress));
}

TEST_F(Cpu6502, ShouldImplementSTXa) {
    sram.clear(0);
    
    const uint16_t kTestAddress = 0x1234;
    const uint8_t kTestData = 0x39;

    Assembler()
        .LDX().immediate(kTestData)
        .STX().a(kTestAddress)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDXi
    testBench.tick(2);
    testBench.trace.clear();

    // test STXa (then NOP)
    testBench.tick(6);

    // note: o_data has valid data for step before and after
    //       write data should be valid at end of phi 2
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_rw).signal("11").repeat(3)      // READ (STXa)
                    .signal("00")               // WRITE (STXa)
                    .signal("11").repeat(2)     // READ (NOP)
        .port(o_data).signal({0}).repeatEachStep(2).repeat(3)
                     .signal({0, kTestData, kTestData, 0})
                     .signal({0}).repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, kTestAddress})    // STXa
                        .signal({5, 6 })                    // NOP
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(12)
        .port(o_debug_x).signal({kTestData}).repeat(6)
                         .repeatEachStep(2)
        .port(o_debug_y).signal({0xFF}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
    EXPECT_EQ(kTestData, sram.read(kTestAddress));
}

TEST_F(Cpu6502, ShouldImplementSTYa) {
    sram.clear(0);
    
    const uint16_t kTestAddress = 0x1234;
    const uint8_t kTestData = 76;

    Assembler()
        .LDY().immediate(kTestData)
        .STY().a(kTestAddress)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDYi
    testBench.tick(2);
    testBench.trace.clear();

    // test STYa (then NOP)
    testBench.tick(6);

    // note: o_data has valid data for step before and after
    //       write data should be valid at end of phi 2
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_rw).signal("11").repeat(3)      // READ (STYa)
                    .signal("00")               // WRITE (STYa)
                    .signal("11").repeat(2)     // READ (NOP)
        .port(o_data).signal({0}).repeatEachStep(2).repeat(3)
                     .signal({0, kTestData, kTestData, 0})
                     .signal({0}).repeatEachStep(2)
        .port(o_address).signal({2, 3, 4, kTestAddress})    // STYa
                        .signal({5, 6 })                    // NOP
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF}).repeat(12)
        .port(o_debug_x).signal({0xFF}).repeat(12)
        .port(o_debug_y).signal({kTestData}).repeat(6)
                         .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
    EXPECT_EQ(kTestData, sram.read(kTestAddress));
}

