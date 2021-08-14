#include "Cpu6502.fixture.hpp"

namespace {
    const std::map<uint8_t, uint8_t> kTestCasesLDA = {
        {0x00, U|Z},
        {1<<7, U|N},
        {1, U}
    };
}

TEST_F(Cpu6502, ShouldImplementLDAi) {
    sram.clear(0);
    
    const uint8_t kTestData = 0x53;

    Assembler()
        .LDA().immediate(kTestData)
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
        .port(o_debug_ac).signal({0x00, kTestData})
                        .repeatEachStep(4)
        .port(o_debug_x).signal({0x00}).repeat(8)
        .port(o_debug_y).signal({0x00}).repeat(8);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDAiProcessorStatus) {
    for (auto& testCase : kTestCasesLDA) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
            .CLI()
            .LDA().immediate(kTestData)
            .NOP()
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(5);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDAa) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x42;

    TestAbsolute<LDA> testAbsolute = {
        .address = kTestAddress,
        .data = kTestData,
        .port = o_debug_ac,
        .expected = kTestData
    };
    
    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLDAaProcessorStatus) {
    for (auto& testCase : kTestCasesLDA) {
        const uint16_t kTestAddress = 0x5678;
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
        
        Assembler()
            .CLI()
            .LDA().a(kTestAddress)
            .NOP()
            .compileTo(sram);

        sram.write(kTestAddress, kTestData);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(8);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDAabsoluteIndexedWithXWithoutCarry) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x42;

    TestAbsoluteIndexed<LDA> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .port = o_debug_ac,
            .expected = kTestData
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLDAabsoluteIndexedWithXProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesLDA) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kX = 3;
    
        Assembler()
                .CLI()
                .LDX().immediate(kX)
                .LDA().a("M").x()
                .NOP()
            .org(0x678A)
            .label("M")
            .org(0x678A + kX)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDAabsoluteIndexedWithXWithCarry) {
    const uint16_t kTestAddress = 0x56FE;
    const uint8_t kTestData = 0x32;

    TestAbsoluteIndexed<LDA> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .port = o_debug_ac,
            .expected = kTestData
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 3,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLDAabsoluteIndexedWithXProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesLDA) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kX = 4;
    
        Assembler()
                .CLI()
                .LDX().immediate(kX)
                .LDA().a("M").x()
                .NOP()
            .org(0xCDFE)
            .label("M")
            .org(0xCDFE + kX)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(11);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDAabsoluteIndexedWithYWithoutCarry) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x32;

    TestAbsoluteIndexed<LDA> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .port = o_debug_ac,
            .expected = kTestData
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLDAabsoluteIndexedWithYProcessorStatusWithoutCarry) {
    for (auto& testCase : kTestCasesLDA) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kY = 3;
    
        Assembler()
                .CLI()
                .LDY().immediate(kY)
                .LDA().a("M").y()
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

TEST_F(Cpu6502, ShouldImplementLDAabsoluteIndexedWithYWithCarry) {
    const uint16_t kTestAddress = 0x56Fe;
    const uint8_t kTestData = 0x32;

    TestAbsoluteIndexed<LDA> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .port = o_debug_ac,
            .expected = kTestData
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 3,
    };

    helperTestInternalExecutionOnMemoryData(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementLDAabsoluteIndexedWithYProcessorStatusWithCarry) {
    for (auto& testCase : kTestCasesLDA) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        const uint8_t kY = 4;
    
        Assembler()
                .CLI()
                .LDY().immediate(kY)
                .LDA().a("M").y()
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

TEST_F(Cpu6502, ShouldImplementLDAZeroPageIndirectIndexedWithY) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0x4;
    const uint8_t kTestData = 0x50;

    Assembler assembler;
    assembler
        .NOP()
        .org(0x0000 + kTestAddressIndirect)
            .word(kTestAddress)
        .org(kTestAddress + kTestOffset)
            .byte(kTestData)
        .org(0xABCD)
        .label("init")
            .LDY().immediate(kTestOffset)
        .label("start")
            .LDA().zpIndirect(kTestAddressIndirect).y()
            .NOP()
        .org(0xFFFC)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // simulate LDY (init)
    testBench.tick(2);
    testBench.trace.clear();

    // simulate LDA (zp),y
    testBench.tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_sync).signal("1000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // LDA (zp),y
                            addressStart.byteIndex(),
                            addressStart.byteIndex()+1u,
                            0x0000 + kTestAddressIndirect,
                            0x0001 + kTestAddressIndirect,
                            kTestAddress + kTestOffset,

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(6)
                         .signal({kTestData}).repeat(1)
                        .concat()
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(7).repeatEachStep(2)
        .port(o_debug_y).signal({kTestOffset}).repeat(7).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDAZeroPageIndirectIndexedWithYWithCarry) {
    sram.clear(0);
    
    const uint8_t kTestAddressIndirect = 42;
    const uint16_t kTestAddress = 0x1230;
    const uint8_t kTestOffset = 0xF4;
    const uint8_t kTestData = 0x50;

    Assembler assembler;
    assembler
        .NOP()
        .org(0x0000 + kTestAddressIndirect)
            .word(kTestAddress)
        .org(kTestAddress + kTestOffset)
            .byte(kTestData)
        .org(0xABCD)
        .label("init")
            .LDY().immediate(kTestOffset)
        .label("start")
            .LDA().zpIndirect(kTestAddressIndirect).y()
            .NOP()
        .org(0xFFFC)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // simulate LDY (init)
    testBench.tick(2);
    testBench.trace.clear();

    // simulate LDA (zp),y
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // LDA (zp),y
                            addressStart.byteIndex(),
                            addressStart.byteIndex()+1u,
                            0x0000 + kTestAddressIndirect,
                            0x0001 + kTestAddressIndirect,
                            (kTestAddress & 0xff00) + ((kTestAddress + kTestOffset) & 0x00ff),  // without carry
                            kTestAddress + kTestOffset,                                         // with carry

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(7)
                         .signal({kTestData}).repeat(1)
                        .concat()
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(8).repeatEachStep(2)
        .port(o_debug_y).signal({kTestOffset}).repeat(8).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDAZeroPageIndirectIndexedWithYProcessorStatus) {
    for (auto& testCase : kTestCasesLDA) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kTestAddressIndirect = 42;
        const uint16_t kTestAddress = 0x1230;
        const uint8_t kTestOffset = 0x04;

        Assembler assembler;
        assembler
            .NOP()
            .org(0x0000 + kTestAddressIndirect)
                .word(kTestAddress)
            .org(kTestAddress + kTestOffset)
                .byte(kTestData)
            .org(0xABCD)
            .label("init")
                .CLI()
                .LDY().immediate(kTestOffset)
            .label("start")
                .LDA().zpIndirect(kTestAddressIndirect).y()
                .NOP()
            .org(0xFFFC)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(11);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDAzeropage) {
    sram.clear(0);
    
    const uint8_t kTestAddress = 0x94;
    const uint8_t kTestData = 0x53;

    Assembler()
            .LDA().zp(kTestAddress)
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
        .port(o_debug_ac).signal({0x00}).repeat(8)
                        .signal({kTestData}).repeat(2)
        .port(o_debug_x).signal({0x00}).repeat(10)
        .port(o_debug_y).signal({0x00}).repeat(10);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDAzeropageProcessorStatus) {
    const uint8_t kTestAddress = 0x78;

    for (auto& testCase : kTestCasesLDA) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDA().zp(kTestAddress)
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

TEST_F(Cpu6502, ShouldImplementLDAzeropageIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestAddress = 0x94;
    const uint8_t kTestData = 0x57;
    const uint8_t kX = 0xfe;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddress + kX) % 0x100);

    Assembler()
            .LDX().immediate(kX)
            .LDA().zp(kTestAddress).x()
            .NOP()
        .org(kTestAddressIndexed)
        .byte(kTestData)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDX
    testBench.tick(2);
    testBench.trace.clear();

    // simulate LDA/NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("11")
                    .repeat(6)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // LDA
                            2, 
                            3, 
                            kTestAddress, 
                            kTestAddressIndexed, 
                            // NOP
                            4, 
                            5})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(5)
                        .signal({kTestData})
                        .concat().repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(12)
        .port(o_debug_x).signal({kX}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDAzeropageIndexedWithXProcessorStatus) {
    const uint8_t kTestAddress = 0x78;
    const uint8_t kX = 0x11;

    for (auto& testCase : kTestCasesLDA) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;
        
        sram.clear(0);
    
        Assembler()
                .CLI()
                .LDX().immediate(kX)
                .LDA().zp(kTestAddress).x()
                .NOP()
            .org(0x0000 + kTestAddress + kX)
            .byte(kTestData)
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(10);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}

TEST_F(Cpu6502, ShouldImplementLDAZeroPageIndirectIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestAddressZeroPage = 0x42;
    const uint8_t kTestOffset = 0x40;
    const uint16_t kTestAddressZeroPageWithOffset = 0x0000 + uint8_t(kTestAddressZeroPage + kTestOffset);
    const uint16_t kTestAddress = 0x1230;    
    const uint8_t kTestData = 0x50;

    Assembler assembler;
    assembler
        .NOP()
        .org(kTestAddressZeroPageWithOffset)
            .word(kTestAddress)
        .org(kTestAddress)
            .byte(kTestData)
        .org(0xABCD)
        .label("init")
            .LDX().immediate(kTestOffset)
        .label("start")
            .LDA().zpIndirect(kTestAddressZeroPage).x()
            .NOP()
        .org(0xFFFC)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // simulate LDX
    testBench.tick(2);
    testBench.trace.clear();

    // simulate LDA (zp,x)
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // LDA (zp,x)
                            addressStart.byteIndex(),
                            addressStart.byteIndex() + 1u,
                            0x0000 + kTestAddressZeroPage,
                            kTestAddressZeroPageWithOffset,
                            kTestAddressZeroPageWithOffset + 1u,
                            kTestAddress,

                            // NOP
                            addressStart.byteIndex() + 2u,
                            addressStart.byteIndex() + 3u
                        })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0}).repeat(7)
                        .signal({kTestData})
                        .concat()
                        .repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(8).repeatEachStep(2)
        .port(o_debug_x).signal({kTestOffset}).repeat(8).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldImplementLDAZeroPageIndirectIndexedWithXProcessorStatus) {
    for (auto& testCase : kTestCasesLDA) {
        const uint8_t kTestData = testCase.first;
        const uint8_t kExpectedProcessorStatus = testCase.second;

        sram.clear(0);

        const uint8_t kTestAddressZeroPage = 0x42;
        const uint8_t kTestOffset = 0x40;
        const uint16_t kTestAddressZeroPageWithOffset = 0x0000 + uint8_t(kTestAddressZeroPage + kTestOffset);
        const uint16_t kTestAddress = 0x1230;    

        Assembler assembler;
        assembler
                .NOP()
            .org(kTestAddressZeroPageWithOffset)
                .word(kTestAddress)
            .org(kTestAddress)
                .byte(kTestData)            
            .org(0xABCD)
            .label("init")
                .CLI()
                .LDX().immediate(kTestOffset)
            .label("start")
                .LDA().zpIndirect(kTestAddressZeroPage).x()
                .NOP()
            .org(0xFFFC)
            .word("init")
            .compileTo(sram);

        testBench.reset();
        helperSkipResetVector();

        testBench.tick(12);
        EXPECT_EQ(kExpectedProcessorStatus, testBench.core().o_debug_p);
    }
}