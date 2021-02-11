#include "Cpu6502.fixture.hpp"

namespace {
    const std::map<uint8_t, uint8_t> kTestCasesLDX = {
        {0x00, Z},
        {1<<7, N},
        {1, 0}
    };
}

TEST_F(Cpu6502, ShouldImplementLDXi) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x53;

    Assembler()
        .LDX().immediate(kTestData)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("1010").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, 3})
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0xFF, kTestData})
                        .repeatEachStep(4)
        .port(o_debug_ac).signal({0xFF}).repeat(8)
        .port(o_debug_y).signal({0xFF}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDXiProcessorStatus) {
    for (auto& testCase : kTestCasesLDX) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .LDX().immediate(kTestData)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(3);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDXa) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x42;

    TestAbsolute<LDX> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .port = o_debug_x,
        .expected = kTestData
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLDXaProcessorStatus) {
    for (auto& testCase : kTestCasesLDX) {
        const uint16_t kTestAddress = 0x5678;
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
        
        Assembler()
            .LDX().a(kTestAddress)
            .NOP()
            .compileTo(sram);

        sram.write(kTestAddress, kTestData);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(6);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDXabsoluteIndexedWithYWithoutCarry) {
    const uint16_t kTestAddress = 0x56Fe;
    const uint8_t kTestData = 0x32;

    TestAbsoluteIndexed<LDX> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .port = o_debug_x,
            .expected = kTestData
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLDXabsoluteIndexedWithYProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesLDX) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kY = 3;
    
        Assembler()
                .LDY().immediate(kY)
                .LDX().a("M").y()
                .NOP()
            .org(0x678A)
            .label("M")
            .org(0x678A + kY)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDXabsoluteIndexedWithYWithCarry) {
    const uint16_t kTestAddress = 0x56Fe;
    const uint8_t kTestData = 0x32;

    TestAbsoluteIndexed<LDX> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .port = o_debug_x,
            .expected = kTestData
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 3,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLDXabsoluteIndexedWithYProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesLDX) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kY = 4;
    
        Assembler()
                .LDY().immediate(kY)
                .LDX().a("M").y()
                .NOP()
            .org(0xCDFE)
            .label("M")
            .org(0xCDFE + kY)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(9);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}
