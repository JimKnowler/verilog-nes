#include "Cpu6502.fixture.hpp"

namespace {
    const std::map<uint8_t, uint8_t> kTestCasesLDX = {
        {0x00, U|Z},
        {1<<7, U|N},
        {1, U}
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
        .port(o_debug_x).signal({0x00, kTestData})
                        .repeatEachStep(4)
        .port(o_debug_ac).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDXiProcessorStatus) {
    for (auto& testCase : kTestCasesLDX) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDX().immediate(kTestData)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(5);
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
            .CLI()
            .LDX().a(kTestAddress)
            .NOP()
            .compileTo(sram);

        sram.write(kTestAddress, kTestData);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
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
                .CLI()
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

        testBench.tick(10);
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
                .CLI()
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

        testBench.tick(11);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDXzeropage) {
    sram.clear(0);
    
    const uint8_t kTestAddress = 0x94;
    const uint8_t kTestData = 0x53;

    Assembler()
            .LDX().zp(kTestAddress)
            .NOP()
        .org(0x0000 + kTestAddress)
        .byte(kTestData)
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_address).signal({0, 1, kTestAddress, 2, 3})
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(8)
                        .signal({kTestData}).repeat(2)
        .port(o_debug_ac).signal({0x00}).repeat(10)
        .port(o_debug_y).signal({0x00}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDXzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0x78;

    for (auto& testCase : kTestCasesLDX) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDX().zp(kTestAddress)
                .NOP()
            .org(0x0000 + kTestAddress)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(7);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDXzeropageIndexedWithY) {
    sram.clear(0);
    
    const uint8_t kTestAddress = 0x94;
    const uint8_t kTestData = 0x57;
    const uint8_t kY = 0xfe;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddress + kY) % 0x100);

    Assembler()
            .LDY().immediate(kY)
            .LDX().zp(kTestAddress).y()
            .NOP()
        .org(kTestAddressIndexed)
        .byte(kTestData)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDY
    testBench.tick(2);
    testBench.trace.clear();

    // simulate LDX/NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDX
                            2, 
                            3, 
                            kTestAddress, 
                            kTestAddressIndexed, 
                            // NOP
                            4, 
                            5})
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(5)
                        .signal({kTestData})
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({kY}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDXzeropageIndexedWithYProcessorStatus) {
    const uint8_t kTestAddress = 0x78;
    const uint8_t kY = 0x11;

    for (auto& testCase : kTestCasesLDX) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDY().immediate(kY)
                .LDX().zp(kTestAddress).y()
                .NOP()
            .org(0x0000 + kTestAddress + kY)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}