#include "Cpu6502.fixture.hpp"

namespace {
    const std::map<uint8_t, uint8_t> kTestCasesROR = {
        {1, U|Z|C},
        {2, U},
        {3, U|C},
        {0, U|Z},
        {0xFF, U|C}
    };

    const std::map<uint8_t, uint8_t> kTestCasesRORwithCarryIn = {
        {1, U|C|N},
        {2, U|N},
        {3, U|C|N},
        {0, U|N},
        {0xFF, U|C|N}
    };

    const std::map<uint8_t, uint8_t> kTestCasesROL = {
        {0, U|Z},
        {1, U},
        {0x40, U|N},
        {0xff, U|C|N},
        {0x80, U|C|Z}
    };

    const std::map<uint8_t, uint8_t> kTestCasesROLwithCarryIn = {
        {0, U},
        {1, U},
        {0x40, U|N},
        {0xff, U|C|N},
        {0x80, U|C}
    };

    const std::map<uint8_t, uint8_t> kTestCasesLSR = {
        {1, U|Z|C},
        {2, U},
        {3, U|C},
        {0, U|Z},
        {0xFF, U|C}
    };

    const std::map<uint8_t, uint8_t> kTestCasesASL = {
        {0, U|Z},
        {1, U},
        {0x40, U|N},
        {0xff, U|C|N},
        {0x80, U|C|Z}
    };
}

TEST_F(Cpu6502, ShouldImplementLSRaccumulator) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x93;

    Assembler()
        .LDA().immediate(kTestData)
        .LSR().A()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate LSR and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(6)
                        .signal({kTestData >> 1}).repeat(2)
        .port(o_debug_x).signal({0x00})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLSRaccumulatorProcessorStatus) {
    for (auto& testCase : kTestCasesLSR) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData)
            .LSR().A()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementASLaccumulator) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x93;

    Assembler()
        .LDA().immediate(kTestData)
        .ASL().A()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate ASL and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(6)
                        .signal({(kTestData << 1) & 0xff}).repeat(2)
        .port(o_debug_x).signal({0x00})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementASLaccumulatorProcessorStatus) {
    for (auto& testCase : kTestCasesASL) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData)
            .ASL().A()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementRORaccumulator) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x93;

    Assembler()
        .LDA().immediate(kTestData)
        .ROR().A()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate ROR and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(6)
                        .signal({kTestData >> 1}).repeat(2)
        .port(o_debug_x).signal({0x00})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementRORaccumulatorProcessorStatus) {
    for (auto& testCase : kTestCasesROR) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData)
            .ROR().A()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementROLaccumulator) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x93;

    Assembler()
        .LDA().immediate(kTestData)
        .ROL().A()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA
    testBench.tick(2);
    testBench.trace.clear();

    // simulate ROL and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({2, 3, 3, 4})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(6)
                        .signal({(kTestData << 1) & 0xff}).repeat(2)
        .port(o_debug_x).signal({0x00})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementROLaccumulatorProcessorStatus) {
    for (auto& testCase : kTestCasesROL) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData)
            .ROL().A()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementRORaccumulatorWithCarryIn) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x93;

    Assembler()
        .LDA().immediate(kTestData)
        .SEC()
        .ROR().A()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA & SEC
    testBench.tick(4);
    testBench.trace.clear();

    // simulate ROR and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({3, 4, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(6)
                        .signal({0x80 | (kTestData >> 1)}).repeat(2)
        .port(o_debug_x).signal({0x00})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementRORaccumulatorWithCarryInProcessorStatus) {
    for (auto& testCase : kTestCasesRORwithCarryIn) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData)
            .SEC()
            .ROR().A()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementROLaccumulatorWithCarryIn) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x93;

    Assembler()
        .LDA().immediate(kTestData)
        .SEC()
        .ROL().A()
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA + SEC
    testBench.tick(4);
    testBench.trace.clear();

    // simulate ROL and NOP
    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({3, 4, 4, 5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(6)
                        .signal({(1 + (kTestData << 1)) & 0xff}).repeat(2)
        .port(o_debug_x).signal({0x00})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementROLaccumulatorWithCarryInProcessorStatus) {
    for (auto& testCase : kTestCasesROLwithCarryIn) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData)
            .SEC()
            .ROL().A()
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLSRabsolute) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsolute<LSR> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .expected = kTestData >> 1
    };

    helperTestReadModifyWrite(testAbsolute);    
}

TEST_F(Cpu6502, ShouldImplementLSRabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesLSR) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LSR().absolute("data")
            .NOP()
        .org(0x2345)
        .label("data")
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementASLabsolute) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsolute<ASL> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .expected = (kTestData << 1) & 0xff
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementASLabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesASL) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .ASL().absolute("data")
            .NOP()
        .org(0x2345)
        .label("data")
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementRORabsolute) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsolute<ROR> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .expected = kTestData >> 1
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementRORabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesROR) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .ROR().absolute("data")
            .NOP()
        .org(0x2345)
        .label("data")
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementROLabsolute) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsolute<ROL> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .expected = (kTestData << 1) & 0xff
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementROLabsoluteProcessorStatus) {
    for (auto& testCase : kTestCasesROL) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .ROL().absolute("data")
            .NOP()
        .org(0x2345)
        .label("data")
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementRORabsoluteWithCarryIn) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsolute<ROR> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .expected = 0x80 | (kTestData >> 1),

        .presetCarry = true
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementRORabsoluteWithCarryInProcessorStatus) {
    for (auto& testCase : kTestCasesRORwithCarryIn) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .SEC()
            .ROR().absolute("data")
            .NOP()
        .org(0x2345)
        .label("data")
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementROLabsoluteWithCarryIn) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsolute<ROL> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .expected = ((kTestData << 1) + 1) & 0xff,

        .presetCarry = true
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementROLabsoluteWithCarryInProcessorStatus) {
    for (auto& testCase : kTestCasesROLwithCarryIn) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .SEC()
            .ROL().absolute("data")
            .NOP()
        .org(0x2345)
        .label("data")
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementRORabsoluteIndexedWithX) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsoluteIndexed<ROR> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .expected = kTestData >> 1
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementRORabsoluteIndexedWithXProcessorStatus) {
    const uint8_t kX = 45;

    for (auto& testCase : kTestCasesROR) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(kX)
            .ROR().absolute("data").x()
            .NOP()
        .org(0x2345)
        .label("data")
        .org(0x2345 + kX)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementROLabsoluteIndexedWithX) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsoluteIndexed<ROL> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .expected = (kTestData << 1) & 0xff
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementROLabsoluteIndexedWithXProcessorStatus) {
    for (auto& testCase : kTestCasesROL) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(kX)
            .ROL().absolute("data").x()
            .NOP()
        .org(0x2345)
        .label("data")
        .org(0x2345 + kX)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementRORabsoluteIndexedWithXWithCarryIn) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsoluteIndexed<ROR> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .expected = 0x80 | (kTestData >> 1),

            .presetCarry = true
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementRORabsoluteIndexedWithXWithCarryInProcessorStatus) {
    const uint8_t kX = 46;

    for (auto& testCase : kTestCasesRORwithCarryIn) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .SEC()
            .LDX().immediate(kX)
            .ROR().absolute("data").x()
            .NOP()
        .org(0x2345)
        .label("data")
        .org(0x2345 + kX)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(14);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementROLabsoluteIndexedWithXWithCarryIn) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsoluteIndexed<ROL> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .expected = ((kTestData << 1) + 1) & 0xff,

            .presetCarry = true
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 6
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementROLabsoluteIndexedWithXWithCarryInProcessorStatus) {
    for (auto& testCase : kTestCasesROLwithCarryIn) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .SEC()
            .LDX().immediate(kX)
            .ROL().absolute("data").x()
            .NOP()
        .org(0x2345)
        .label("data")
        .org(0x2345 + kX)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(14);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementROLzeropage) {
    const uint8_t kTestAddress = 0x78;
    const uint8_t kTestData = 0x49;

    sram.clear(0);
    
    Assembler assembler;
    assembler
            .ROL().zp(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

    testBench.reset();
    helperSkipResetVector();

    testBench.tick(7);
    
    Trace expected = TraceBuilder()
        .port(o_address).signal({
            // ROL
            0,
            1,
            0x0000 + kTestAddress,      // R
            0x0000 + kTestAddress,      // W (original value)
            0x0000 + kTestAddress,      // W (incremented value)

            // NOP
            2,
            3
        }).repeatEachStep(2)
        .port(o_rw).signal("1110011").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(7)
            .signal({kTestData}).repeat(2)
            .signal({(kTestData << 1) & 0xff}).repeat(2)
            .signal({0}).repeat(3)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(7).repeatEachStep(2);
    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementROLzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0x87;

    for (auto& testCase : kTestCasesROL) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .ROL().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementROLzeropageWithCarryIn) {
    const uint8_t kTestAddress = 0x78;
    const uint8_t kTestData = 0x49;

    sram.clear(0);
    
    Assembler assembler;
    assembler
            .SEC()
            .ROL().zp(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

    testBench.reset();
    helperSkipResetVector();

    // skip SEC
    testBench.tick(2);
    testBench.trace.clear();

    // simulate ROL + NOP
    testBench.tick(7);
    
    Trace expected = TraceBuilder()
        .port(o_address).signal({
            // ROL
            1,
            2,
            0x0000 + kTestAddress,      // R
            0x0000 + kTestAddress,      // W (original value)
            0x0000 + kTestAddress,      // W (incremented value)

            // NOP
            3,
            4
        }).repeatEachStep(2)
        .port(o_rw).signal("1110011").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(7)
            .signal({kTestData}).repeat(2)
            .signal({((kTestData << 1) + 1) & 0xff}).repeat(2)
            .signal({0}).repeat(3)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(7).repeatEachStep(2);
    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementROLzeropageWithCarryInProcessorStatus) {
    const uint8_t kTestAddress = 0x87;

    for (auto& testCase : kTestCasesROLwithCarryIn) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .SEC()
            .ROL().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(11);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementRORzeropage) {
    const uint8_t kTestAddress = 0x78;
    const uint8_t kTestData = 0x49;

    sram.clear(0);
    
    Assembler assembler;
    assembler
            .ROR().zp(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

    testBench.reset();
    helperSkipResetVector();

    testBench.tick(7);
    
    Trace expected = TraceBuilder()
        .port(o_address).signal({
            // ROR
            0,
            1,
            0x0000 + kTestAddress,      // R
            0x0000 + kTestAddress,      // W (original value)
            0x0000 + kTestAddress,      // W (incremented value)

            // NOP
            2,
            3
        }).repeatEachStep(2)
        .port(o_rw).signal("1110011").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(7)
            .signal({kTestData}).repeat(2)
            .signal({(kTestData >> 1) & 0xff}).repeat(2)
            .signal({0}).repeat(3)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(7).repeatEachStep(2);
    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementRORzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0x87;

    for (auto& testCase : kTestCasesROR) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .ROR().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementRORzeropageWithCarryIn) {
    const uint8_t kTestAddress = 0x78;
    const uint8_t kTestData = 0x49;

    sram.clear(0);
    
    Assembler assembler;
    assembler
            .SEC()
            .ROR().zp(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

    testBench.reset();
    helperSkipResetVector();

    // skip SEC
    testBench.tick(2);
    testBench.trace.clear();

    // simulate ROL + NOP
    testBench.tick(7);
    
    Trace expected = TraceBuilder()
        .port(o_address).signal({
            // ROR
            1,
            2,
            0x0000 + kTestAddress,      // R
            0x0000 + kTestAddress,      // W (original value)
            0x0000 + kTestAddress,      // W (incremented value)

            // NOP
            3,
            4
        }).repeatEachStep(2)
        .port(o_rw).signal("1110011").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(7)
            .signal({kTestData}).repeat(2)
            .signal({0x80 | (kTestData >> 1)}).repeat(2)
            .signal({0}).repeat(3)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(7).repeatEachStep(2);
    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementRORzeropageWithCarryInProcessorStatus) {
    const uint8_t kTestAddress = 0x87;

    for (auto& testCase : kTestCasesRORwithCarryIn) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .SEC()
            .ROR().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(11);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLSRzeropage) {
    const uint8_t kTestAddress = 0x78;
    const uint8_t kTestData = 0x49;

    sram.clear(0);
    
    Assembler assembler;
    assembler
            .SEC()                  // make sure carry-in is ignored
            .LSR().zp(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

    testBench.reset();
    helperSkipResetVector();

    // skip SEC
    testBench.tick(2);
    testBench.trace.clear();

    // simulate LSR and NOP
    testBench.tick(7);
    
    Trace expected = TraceBuilder()
        .port(o_address).signal({
            // LSR
            1,
            2,
            0x0000 + kTestAddress,      // R
            0x0000 + kTestAddress,      // W (original value)
            0x0000 + kTestAddress,      // W (incremented value)

            // NOP
            3,
            4
        }).repeatEachStep(2)
        .port(o_rw).signal("1110011").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(7)
            .signal({kTestData}).repeat(2)
            .signal({(kTestData >> 1) & 0xff}).repeat(2)
            .signal({0}).repeat(3)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(7).repeatEachStep(2);
    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLSRzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0xa6;

    for (auto& testCase : kTestCasesLSR) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .SEC()                  // make sure carry-in doesn't affect result
            .LSR().zp(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(11);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementASLzeropage) {
    const uint8_t kTestAddress = 0x78;
    const uint8_t kTestData = 0x49;

    sram.clear(0);
    
    Assembler assembler;
    assembler
            .SEC()                  // make sure carry-in is ignored
            .ASL().zp(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

    testBench.reset();
    helperSkipResetVector();

    // skip SEC
    testBench.tick(2);
    testBench.trace.clear();

    // simulate LSR and NOP
    testBench.tick(7);
    
    Trace expected = TraceBuilder()
        .port(o_address).signal({
            // ASL
            1,
            2,
            0x0000 + kTestAddress,      // R
            0x0000 + kTestAddress,      // W (original value)
            0x0000 + kTestAddress,      // W (incremented value)

            // NOP
            3,
            4
        }).repeatEachStep(2)
        .port(o_rw).signal("1110011").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(7)
            .signal({kTestData}).repeat(2)
            .signal({(kTestData << 1) & 0xff}).repeat(2)
            .signal({0}).repeat(3)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(7).repeatEachStep(2);
    
    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementASLzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0xa6;

    for (auto& testCase : kTestCasesASL) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .SEC()                  // make sure carry-in doesn't affect result
            .ASL().zp(kTestAddress)
            .NOP()
        .org(kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(11);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLSRabsoluteIndexedWithX) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsoluteIndexed<LSR> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .expected = kTestData >> 1
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLSRabsoluteIndexedWithXProcessorStatus) {
    const uint8_t kX = 45;

    for (auto& testCase : kTestCasesLSR) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(kX)
            .LSR().absolute("data").x()
            .NOP()
        .org(0x2345)
        .label("data")
        .org(0x2345 + kX)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementASLabsoluteIndexedWithX) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsoluteIndexed<ASL> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .expected = (kTestData << 1) & 0xff
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementASLabsoluteIndexedWithXProcessorStatus) {
    const uint8_t kX = 5;

    for (auto& testCase : kTestCasesASL) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(kX)
            .ASL().absolute("data").x()
            .NOP()
        .org(0x2345)
        .label("data")
        .org(0x2345 + kX)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLSRabsoluteIndexedWithXWithCarryIn) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsoluteIndexed<LSR> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .expected = (kTestData >> 1),

            .presetCarry = true
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLSRabsoluteIndexedWithXWithCarryInProcessorStatus) {
    const uint8_t kX = 46;

    for (auto& testCase : kTestCasesLSR) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .SEC()
            .LDX().immediate(kX)
            .LSR().absolute("data").x()
            .NOP()
        .org(0x2345)
        .label("data")
        .org(0x2345 + kX)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(14);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementASLabsoluteIndexedWithXWithCarryIn) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x93;

    TestAbsoluteIndexed<ASL> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .expected = (kTestData << 1) & 0xff,

            .presetCarry = true
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 6
    };

    helperTestReadModifyWrite(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementASLabsoluteIndexedWithXWithCarryInProcessorStatus) {
    const uint8_t kX = 5;
    
    for (auto& testCase : kTestCasesASL) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .SEC()
            .LDX().immediate(kX)
            .ASL().absolute("data").x()
            .NOP()
        .org(0x2345)
        .label("data")
        .org(0x2345 + kX)
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(14);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}


// todo: ROL a,x + ROR a,x - test when address+index carries