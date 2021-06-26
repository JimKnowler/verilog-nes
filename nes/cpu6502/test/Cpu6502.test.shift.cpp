#include "Cpu6502.fixture.hpp"

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
        .port(o_debug_x).signal({0xFF})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLSRaccumulatorProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {1, Z | C},
        {2, 0},
        {3, C},
        {0, Z},
        {0xFF, C}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
        .LDA().immediate(kTestData)
        .LSR().A()
        .NOP()
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
        .port(o_debug_x).signal({0xFF})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementASLaccumulatorProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0, Z},
        {1, 0},
        {0x40, N},
        {0xff, C|N},
        {0x80, C|Z}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
        .LDA().immediate(kTestData)
        .ASL().A()
        .NOP()
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);

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
        .port(o_debug_x).signal({0xFF})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementRORaccumulatorProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {1, Z | C},
        {2, 0},
        {3, C},
        {0, Z},
        {0xFF, C}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
        .LDA().immediate(kTestData)
        .ROR().A()
        .NOP()
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
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
        .port(o_debug_x).signal({0xFF})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementROLaccumulatorProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0, Z},
        {1, 0},
        {0x40, N},
        {0xff, C|N},
        {0x80, C|Z}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
        .LDA().immediate(kTestData)
        .ROL().A()
        .NOP()
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);

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
        .port(o_debug_x).signal({0xFF})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementRORaccumulatorWithCarryInProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {1, C | N},
        {2, N},
        {3, C | N},
        {0, N},
        {0xFF, C | N}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
        .LDA().immediate(kTestData)
        .SEC()
        .ROR().A()
        .NOP()
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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
        .port(o_debug_x).signal({0xFF})
                        .repeatEachStep(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementROLaccumulatorWithCarryInProcessorStatus) {
    const std::map<uint8_t, uint8_t> testCases = {
        {0, 0},
        {1, 0},
        {0x40, N},
        {0xff, C|N},
        {0x80, C}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
        .LDA().immediate(kTestData)
        .SEC()
        .ROL().A()
        .NOP()
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);

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
    const std::map<uint8_t, uint8_t> testCases = {
        {1, Z | C},
        {2, 0},
        {3, C},
        {0, Z},
        {0xFF, C}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LSR().absolute("data")
            .NOP()
        .org(0x2345)
        .label("data")
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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
    const std::map<uint8_t, uint8_t> testCases = {
        {0, Z},
        {1, 0},
        {0x40, N},
        {0xff, C|N},
        {0x80, C|Z}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .ASL().absolute("data")
            .NOP()
        .org(0x2345)
        .label("data")
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);

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
    const std::map<uint8_t, uint8_t> testCases = {
        {1, Z | C},
        {2, 0},
        {3, C},
        {0, Z},
        {0xFF, C}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .ROR().absolute("data")
            .NOP()
        .org(0x2345)
        .label("data")
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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
    const std::map<uint8_t, uint8_t> testCases = {
        {0, Z},
        {1, 0},
        {0x40, N},
        {0xff, C|N},
        {0x80, C|Z}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .ROL().absolute("data")
            .NOP()
        .org(0x2345)
        .label("data")
        .byte(kTestData)
        .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);

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
    const std::map<uint8_t, uint8_t> testCases = {
        {1, C | N},
        {2, N},
        {3, C | N},
        {0, N},
        {0xFF, C | N}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .SEC()
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
    const std::map<uint8_t, uint8_t> testCases = {
        {0, 0},
        {1, 0},
        {0x40, N},
        {0xff, C|N},
        {0x80, C}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .SEC()
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
    const std::map<uint8_t, uint8_t> testCases = {
        {1, Z | C},
        {2, 0},
        {3, C},
        {0, Z},
        {0xFF, C}
    };

    const uint8_t kX = 45;

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
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

        testBench.tick(10);
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
    const std::map<uint8_t, uint8_t> testCases = {
        {0, Z},
        {1, 0},
        {0x40, N},
        {0xff, C|N},
        {0x80, C|Z}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
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

        testBench.tick(10);
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
    const std::map<uint8_t, uint8_t> testCases = {
        {1, C | N},
        {2, N},
        {3, C | N},
        {0, N},
        {0xFF, C | N}
    };

    const uint8_t kX = 46;

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
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

        testBench.tick(12);
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
    const std::map<uint8_t, uint8_t> testCases = {
        {0, 0},
        {1, 0},
        {0x40, N},
        {0xff, C|N},
        {0x80, C}
    };

    for (auto& testCase : testCases) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
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

        testBench.tick(12);

        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}
