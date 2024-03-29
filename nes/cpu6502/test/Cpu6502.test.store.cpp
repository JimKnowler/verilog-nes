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
        .port(o_debug_ac).signal({0x00}).repeat(12)
        .port(o_debug_x).signal({kTestData}).repeat(6)
                         .repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(12);

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
        .port(o_debug_ac).signal({0x00}).repeat(12)
        .port(o_debug_x).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({kTestData}).repeat(6)
                         .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
    EXPECT_EQ(kTestData, sram.read(kTestAddress));
}

TEST_F(Cpu6502, ShouldImplementSTAabsoluteIndexedWithXwithoutCarry) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x42;

    TestAbsoluteIndexed<STA> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .port = o_debug_ac,
            .expected = kTestData,
            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 5,
    };

    helperTestStore(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSTAabsoluteIndexedWithXwithCarry) {
    const uint16_t kTestAddress = 0x56FE;
    const uint8_t kTestData = 0x32;

    TestAbsoluteIndexed<STA> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .port = o_debug_ac,
            .expected = kTestData,
            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData
        },

        .indexRegister = kX,
        .preloadIndexRegisterValue = 3,
    };

    helperTestStore(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSTAabsoluteIndexedWithYwithoutCarry) {
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x42;

    TestAbsoluteIndexed<STA> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .port = o_debug_ac,
            .expected = kTestData,
            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 5,
    };

    helperTestStore(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSTAabsoluteIndexedWithYwithCarry) {
    const uint16_t kTestAddress = 0x56FE;
    const uint8_t kTestData = 0x32;

    TestAbsoluteIndexed<STA> testAbsolute = {
        {
            .address = kTestAddress,
            .data = kTestData,
            .port = o_debug_ac,
            .expected = kTestData,
            .preloadPort = &o_debug_ac,
            .preloadPortValue = kTestData
        },

        .indexRegister = kY,
        .preloadIndexRegisterValue = 3,
    };

    helperTestStore(testAbsolute);
}

TEST_F(Cpu6502, ShouldImplementSTAzeropage) {
    sram.clear(0);
    
    const uint16_t kTestAddress = 0x0034;
    const uint8_t kTestData = 0x42;

    Assembler()
        .LDA().immediate(kTestData)
        .STA().zp(kTestAddress)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAi
    testBench.tick(2);
    testBench.trace.clear();

    // test STA (then NOP)
    testBench.tick(5);

    // note: o_data has valid data for step before and after
    //       write data should be valid at end of phi 2
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_rw).signal("11").repeat(2)      // READ (STA)
                    .signal("00")               // WRITE (STA)
                    .signal("11").repeat(2)     // READ (NOP)
        .port(o_data).signal({0}).repeatEachStep(2).repeat(2)
                     .signal({0, kTestData, kTestData, 0})
                     .signal({0}).repeatEachStep(2)
        .port(o_address).signal({2, 3, kTestAddress})    // STA
                        .signal({4, 5 })                 // NOP
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(5)
                         .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
    EXPECT_EQ(kTestData, sram.read(kTestAddress));
}

TEST_F(Cpu6502, ShouldImplementSTXzeropage) {
    sram.clear(0);
    
    const uint16_t kTestAddress = 0x0034;
    const uint8_t kTestData = 0x42;

    Assembler()
        .LDX().immediate(kTestData)
        .STX().zp(kTestAddress)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDXi
    testBench.tick(2);
    testBench.trace.clear();

    // test STX (then NOP)
    testBench.tick(5);

    // note: o_data has valid data for step before and after
    //       write data should be valid at end of phi 2
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_rw).signal("11").repeat(2)      // READ (STX)
                    .signal("00")               // WRITE (STX)
                    .signal("11").repeat(2)     // READ (NOP)
        .port(o_data).signal({0}).repeatEachStep(2).repeat(2)
                     .signal({0, kTestData, kTestData, 0})
                     .signal({0}).repeatEachStep(2)
        .port(o_address).signal({2, 3, kTestAddress})    // STX
                        .signal({4, 5 })                 // NOP
                        .concat().repeatEachStep(2)
        .port(o_debug_x).signal({kTestData}).repeat(5)
                         .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
    EXPECT_EQ(kTestData, sram.read(kTestAddress));
}

TEST_F(Cpu6502, ShouldImplementSTYzeropage) {
    sram.clear(0);
    
    const uint16_t kTestAddress = 0x0034;
    const uint8_t kTestData = 0x42;

    Assembler()
        .LDY().immediate(kTestData)
        .STY().zp(kTestAddress)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDYi
    testBench.tick(2);
    testBench.trace.clear();

    // test STY (then NOP)
    testBench.tick(5);

    // note: o_data has valid data for step before and after
    //       write data should be valid at end of phi 2
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_sync).signal("10010").repeatEachStep(2)
        .port(o_rw).signal("11").repeat(2)      // READ (STY)
                    .signal("00")               // WRITE (STY)
                    .signal("11").repeat(2)     // READ (NOP)
        .port(o_data).signal({0}).repeatEachStep(2).repeat(2)
                     .signal({0, kTestData, kTestData, 0})
                     .signal({0}).repeatEachStep(2)
        .port(o_address).signal({2, 3, kTestAddress})    // STY
                        .signal({4, 5 })                 // NOP
                        .concat().repeatEachStep(2)
        .port(o_debug_y).signal({kTestData}).repeat(5)
                         .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
    EXPECT_EQ(kTestData, sram.read(kTestAddress));
}

TEST_F(Cpu6502, ShouldImplementSTAzeropageIndirectIndexedWithYWithoutCarry) {
    sram.clear(0);
    
    const uint16_t kTestAddress = 0x0035;
    const uint8_t kTestData = 0x53;
    const uint8_t kY = 0x22;

    Assembler()
            .LDA().immediate(kTestData)
            .LDY().immediate(kY)
            .STA().zpIndirect("indirect").y()
            .NOP()
        .org(0x0050)
        .label("indirect")
            .word(kTestAddress)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAi, LDYi
    testBench.tick(4);
    testBench.trace.clear();

    // test STA (then NOP)
    testBench.tick(8);

    // note: o_data has valid data for step before and after
    //       write data should be valid at end of phi 2
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_rw).signal("11").repeat(5)      // READ (STA)
                    .signal("00")               // WRITE (STA)
                    .signal("11").repeat(2)     // READ (NOP)
        .port(o_data).signal({0}).repeatEachStep(2).repeat(5)
                     .signal({0, kTestData, kTestData, 0})
                     .signal({0}).repeatEachStep(2)
        .port(o_address).signal({
                            // STA
                            4,
                            5,
                            0x0050,
                            0x0051,
                            kTestAddress+kY,
                            kTestAddress+kY,
                            // NOP
                            6,
                            7 
                        })
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(8)
                        .repeatEachStep(2)
        .port(o_debug_y).signal({kY}).repeat(8)
                        .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
    EXPECT_EQ(kTestData, sram.read(kTestAddress+kY));
}

TEST_F(Cpu6502, ShouldImplementSTAzeropageIndirectIndexedWithYWithCarry) {
    sram.clear(0);
    
    const uint16_t kTestAddress = 0x00FE;
    const uint8_t kTestData = 0x53;
    const uint8_t kY = 0x22;

    Assembler()
            .LDA().immediate(kTestData)
            .LDY().immediate(kY)
            .STA().zpIndirect("indirect").y()
            .NOP()
        .org(0x0050)
        .label("indirect")
            .word(kTestAddress)
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDAi, LDYi
    testBench.tick(4);
    testBench.trace.clear();

    // test STA (then NOP)
    testBench.tick(8);

    // note: o_data has valid data for step before and after
    //       write data should be valid at end of phi 2
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_rw).signal("11").repeat(5)      // READ (STA)
                    .signal("00")               // WRITE (STA)
                    .signal("11").repeat(2)     // READ (NOP)
        .port(o_data).signal({0}).repeatEachStep(2).repeat(5)
                     .signal({0, kTestData, kTestData, 0})
                     .signal({0}).repeatEachStep(2)
        .port(o_address).signal({
                            // STA
                            4,
                            5,
                            0x0050,
                            0x0051,
                            (kTestAddress & 0xff00u) | ((kTestAddress+kY) & 0x00ffu),       // without carry
                            kTestAddress+kY,                                                // with carry
                            // NOP
                            6,
                            7 
                        })
                        .concat().repeatEachStep(2)
        .port(o_debug_ac).signal({kTestData}).repeat(8)
                        .repeatEachStep(2)
        .port(o_debug_y).signal({kY}).repeat(8)
                        .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
    EXPECT_EQ(kTestData, sram.read(kTestAddress+kY));
}

TEST_F(Cpu6502, ShouldImplementSTYzeropageIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestAddress = 0x94;
    const uint8_t kTestData = 0x57;
    const uint8_t kX = 0xfe;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddress + kX) % 0x100);

    Assembler()
            .LDY().immediate(kTestData)
            .LDX().immediate(kX)
            .STY().zp(kTestAddress).x()
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDY/LDX
    testBench.tick(4);
    testBench.trace.clear();

    // simulate STY/NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("111011")
                    .repeatEachStep(2)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // STY
                            4, 
                            5, 
                            kTestAddress, 
                            kTestAddressIndexed, 
                            // NOP
                            6, 
                            7})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({kTestData}).repeat(12)
        .port(o_debug_x).signal({kX}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    EXPECT_EQ(kTestData, sram.read(kTestAddressIndexed));
}

TEST_F(Cpu6502, ShouldImplementSTAzeropageIndexedWithX) {
    sram.clear(0);
    
    const uint8_t kTestAddress = 0x94;
    const uint8_t kTestData = 0x57;
    const uint8_t kX = 0xfe;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddress + kX) % 0x100);

    Assembler()
            .LDA().immediate(kTestData)
            .LDX().immediate(kX)
            .STA().zp(kTestAddress).x()
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDA/LDX
    testBench.tick(4);
    testBench.trace.clear();

    // simulate STA/NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("111011")
                    .repeatEachStep(2)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // STA
                            4, 
                            5, 
                            kTestAddress, 
                            kTestAddressIndexed, 
                            // NOP
                            6, 
                            7})
                        .repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(12)
        .port(o_debug_ac).signal({kTestData}).repeat(12)
        .port(o_debug_x).signal({kX}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    EXPECT_EQ(kTestData, sram.read(kTestAddressIndexed));
}

TEST_F(Cpu6502, ShouldImplementSTXzeropageIndexedWithY) {
    sram.clear(0);
    
    const uint8_t kTestAddress = 0x94;
    const uint8_t kTestData = 0x57;
    const uint8_t kY = 0xfe;
    const uint16_t kTestAddressIndexed = 0x0000 + ((kTestAddress + kY) % 0x100);

    Assembler()
            .LDX().immediate(kTestData)
            .LDY().immediate(kY)
            .STX().zp(kTestAddress).y()
            .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    // skip LDX/LDY
    testBench.tick(4);
    testBench.trace.clear();

    // simulate STX/NOP
    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(6)
        .port(o_rw).signal("111011")
                    .repeatEachStep(2)
        .port(o_sync).signal("100010").repeatEachStep(2)
        .port(o_address).signal({
                            // STX
                            4, 
                            5, 
                            kTestAddress, 
                            kTestAddressIndexed, 
                            // NOP
                            6, 
                            7})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x00}).repeat(12)
        .port(o_debug_y).signal({kY}).repeat(12)
        .port(o_debug_x).signal({kTestData}).repeat(12);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    EXPECT_EQ(kTestData, sram.read(kTestAddressIndexed));
}

TEST_F(Cpu6502, ShouldImplementSTAZeroPageIndirectIndexedWithX) {
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
            .byte(0xff)
        .org(0xABCD)
        .label("init")
            .LDX().immediate(kTestOffset)
            .LDA().immediate(kTestData)
        .label("start")
            .STA().zpIndirect(kTestAddressZeroPage).x()
            .NOP()
        .org(0xFFFC)
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    cpu6502::assembler::Address addressStart("start");
    assembler.lookupAddress(addressStart);

    // simulate LDX, LDA
    testBench.tick(4);
    testBench.trace.clear();

    // simulate STA (zp,x)
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(8)
        .port(o_rw).signal("11").repeat(5)
                    .signal("00")
                    .signal("11").repeat(2)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_debug_tcu).signal({0,1,2,3,4,5,0,1}).repeatEachStep(2)
        .port(o_address).signal({
                            // STA (zp,x)
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
        .port(o_debug_ac).signal({kTestData}).repeat(8)
                        .repeatEachStep(2)
        .port(o_debug_y).signal({0x00}).repeat(8).repeatEachStep(2)
        .port(o_debug_x).signal({kTestOffset}).repeat(8).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    EXPECT_THAT(sram.read(kTestAddress), kTestData);
}