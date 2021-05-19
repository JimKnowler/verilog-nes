#include "Cpu6502.fixture.hpp"

TEST_F(Cpu6502, ShouldImplementBRK) {
    const auto& core = testBench.core();

    sram.clear(0);

    Assembler assembler;
    assembler
            .NOP()
        .org(0x1234)
        .label("init")
            .SEC()                  // non-zero Processor Status (P)
            .SEI()
        .label("start")
            .BRK()
        .label("return_to")
        .org(0x3456)
        .label("interrupt")
            .NOP()
        .org(0xFFFC)                // RESET VECTOR
        .word("init")
        .org(0xFFFE)                // INTERRUPT VECTOR
        .word("interrupt")
        .compileTo(sram);

    helperSkipResetVector();

    // skip init section
    testBench.tick(4);
    testBench.trace.clear();

    // capture information from the assembler / simulated core
    cpu6502::assembler::Address start("start");
    assembler.lookupAddress(start);

    cpu6502::assembler::Address interrupt("interrupt");
    assembler.lookupAddress(interrupt);

    cpu6502::assembler::Address returnTo("return_to");
    assembler.lookupAddress(returnTo);

    const uint8_t sp = core.o_debug_s;
    const uint8_t p = C | I;

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(9)
        .port(o_rw).signal("110001111").repeatEachStep(2)
        .port(o_sync).signal("100000010").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(5)
            .signal({returnTo.hi()}).repeat(2)
            .signal({returnTo.lo()}).repeat(2)
            .signal({p}).repeat(2)
            .signal({0}).repeat(7)
        .port(o_address)
            .signal({
                // BRK - at 'start'
                start.byteIndex(),
                start.byteIndex() + 1u,
                0x0100u + sp,
                0x0100u + sp - 1u,
                0x0100u + sp - 2u,
                0xFFFE,                     // Interrupt Vector (low byte)
                0xFFFF,                     // Interrupt Vector (high byte)

                // NOP (at interrupt vector)
                interrupt.byteIndex(),
                interrupt.byteIndex() + 1u
            }).repeatEachStep(2)
        .port(o_debug_s)
            .signal({sp}).repeat(5)
            .signal({sp - 3u}).repeat(4)
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    // processor flags should be unaffected, apart from B=Break
    EXPECT_EQ(p | B, core.o_debug_p);
}

TEST_F(Cpu6502, ShouldImplementRTI) {
    const auto& core = testBench.core();

    sram.clear(0);

    Assembler assembler;
    assembler
            .NOP()
        .org(0x1234)
        .label("init")
            .SEC()                  // non-zero Processor Status (P)
            .SEI()                  // P = I | C
        .label("start")
            .BRK()
        .label("return_to")
            .NOP()
            .NOP()
        .org(0x3456)
        .label("interrupt")
            .NOP()
            .NOP()
        .label("return_from")
            .RTI()
        .org(0xfffc)                // RESET VECTOR
        .word("init")
        .org(0xFFFE)                // INTERRUPT VECTOR
        .word("interrupt")
        .compileTo(sram);

    // capture information from the assembler
    cpu6502::assembler::Address start("start");
    assembler.lookupAddress(start);

    cpu6502::assembler::Address interrupt("interrupt");
    assembler.lookupAddress(interrupt);

    cpu6502::assembler::Address returnTo("return_to");
    assembler.lookupAddress(returnTo);

    cpu6502::assembler::Address returnFrom("return_from");
    assembler.lookupAddress(returnFrom);

    // skip RESET
    helperSkipResetVector();

    // skip init section
    testBench.tick(4);

    // skip BRK
    testBench.tick(7);

    // capture SP after BRK
    const uint8_t sp = core.o_debug_s;

    // skip 2 x NOPs in 'interrupt' section
    testBench.tick(4);

    // verify that PC is where we expect
    EXPECT_EQ(returnFrom.lo(), core.o_debug_pcl);
    EXPECT_EQ(returnFrom.hi(), core.o_debug_pch);

    // simulate RTI and NOP 
    testBench.trace.clear();
    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(8)
        .port(o_rw).signal("11").repeat(8)
        .port(o_sync).signal("10000010").repeatEachStep(2)
        .port(o_address)
            .signal({
                // RTI - at 'return_from'
                returnFrom.byteIndex(),
                returnFrom.byteIndex() + 1u,
                0x0100u + sp,
                0x0100u + sp + 1u,
                0x0100u + sp + 2u,
                0x0100u + sp + 3u,

                // NOP - at 'return_to'
                returnTo.byteIndex(),
                returnTo.byteIndex() + 1u
            }).repeatEachStep(2)
        .port(o_debug_s)
            .signal({sp}).repeat(5)
            .signal({returnTo.lo()})        // todo: shouldn't be sensitive to internal workings
            .signal({sp + 3u}).repeat(2)
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    // processor flags should be restored
    EXPECT_EQ(I | C, core.o_debug_p);
}

TEST_F(Cpu6502, ShouldImplementIRQ) {
    auto& core = testBench.core();

    sram.clear(0);

    Assembler assembler;
    assembler
            .NOP()
        .org(0x1234)
        .label("init")
            .SEC()                  // non-zero Processor Status (P)
            .SED()
        .label("start")
            .NOP()
        .label("return_to")
        .org(0x3456)
        .label("interrupt")
            .NOP()
        .org(0xFFFC)                // RESET VECTOR
        .word("init")
        .org(0xFFFE)                // INTERRUPT VECTOR
        .word("interrupt")
        .compileTo(sram);

    helperSkipResetVector();

    // skip init section
    testBench.tick(4);
    testBench.trace.clear();

    // capture information from the assembler / simulated core
    cpu6502::assembler::Address start("start");
    assembler.lookupAddress(start);

    cpu6502::assembler::Address interrupt("interrupt");
    assembler.lookupAddress(interrupt);

    cpu6502::assembler::Address returnTo("return_to");
    assembler.lookupAddress(returnTo);

    const uint8_t sp = core.o_debug_s;
    const uint8_t p = C | D;

    // activate the IRQ
    testBench.tick(1);
    core.i_irq_n = 0;
    testBench.tick(10);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(11)
        .port(o_rw).signal("11110001111").repeatEachStep(2)
        .port(o_sync).signal("10100000010").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(9)
            .signal({returnTo.hi()}).repeat(2)
            .signal({returnTo.lo()}).repeat(2)
            .signal({p}).repeat(2)
            .signal({0}).repeat(7)
        .port(o_address)
            .signal({
                // NOP - at 'start'
                start.byteIndex(),
                start.byteIndex() + 1u,

                // IRQ
                returnTo.byteIndex(),
                returnTo.byteIndex(),
                0x0100u + sp,
                0x0100u + sp - 1u,
                0x0100u + sp - 2u,
                0xFFFE,                     // Interrupt Vector (low byte)
                0xFFFF,                     // Interrupt Vector (high byte)

                // NOP (at interrupt vector)
                interrupt.byteIndex(),
                interrupt.byteIndex() + 1u
            }).repeatEachStep(2)
        .port(o_debug_s)
            .signal({sp}).repeat(7)
            .signal({sp - 3u}).repeat(4)
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    // processor flags should be augmented with I=Interrupt
    EXPECT_EQ(p | I, core.o_debug_p);
}

TEST_F(Cpu6502, ShouldIgnoreIRQWhenProcessorStatusHasI) {
    auto& core = testBench.core();

    sram.clear(0);

    Assembler assembler;
    assembler
            .NOP()
        .org(0x1234)
        .label("init")
            .SEI()                  // Ignore interrupts
        .label("start")
            .NOP()
            .NOP()
            .NOP()
            .NOP()
        .org(0x3456)
        .label("interrupt")
            .NOP()
        .org(0xFFFC)                // RESET VECTOR
        .word("init")
        .org(0xFFFE)                // INTERRUPT VECTOR
        .word("interrupt")
        .compileTo(sram);

    helperSkipResetVector();

    // skip init section
    testBench.tick(2);
    testBench.trace.clear();

    // capture information from the assembler / simulated core
    cpu6502::assembler::Address start("start");
    assembler.lookupAddress(start);

    const uint8_t sp = core.o_debug_s;

    // activate the IRQ
    testBench.tick(1);
    core.i_irq_n = 0;
    testBench.tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(6)
        .port(o_rw).signal("111111").repeatEachStep(2)
        .port(o_sync).signal("101010").repeatEachStep(2)
        .port(o_address)
            .signal({
                // 1st NOP - at 'start'
                start.byteIndex(),
                start.byteIndex() + 1u,

                // 2nd NOP
                start.byteIndex() + 1u,
                start.byteIndex() + 2u,

                // 3rd NOP
                start.byteIndex() + 2u,
                start.byteIndex() + 3u,
            }).repeatEachStep(2)
        .port(o_debug_s)
            .signal({sp}).repeat(6)
            .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    EXPECT_EQ(I, core.o_debug_p);
}

TEST_F(Cpu6502, ShouldImplementNMI) {
    auto& core = testBench.core();

    sram.clear(0);

    Assembler assembler;
    assembler
            .NOP()
        .org(0x1234)
        .label("init")
            .SEC()                  // non-zero Processor Status (P)
            .SED()
        .label("start")
            .NOP()
        .label("return_to")
            .NOP()
            .NOP()
        .org(0x3456)
        .label("non_maskable_interrupt")
            .NOP()
        .org(0xFFFA)                // INTERRUPT VECTOR
        .word("non_maskable_interrupt")
        .org(0xFFFC)                // RESET VECTOR
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    // skip init section
    testBench.tick(4);
    testBench.trace.clear();

    // capture information from the assembler / simulated core
    cpu6502::assembler::Address start("start");
    assembler.lookupAddress(start);

    cpu6502::assembler::Address nonMaskableInterrupt("non_maskable_interrupt");
    assembler.lookupAddress(nonMaskableInterrupt);

    cpu6502::assembler::Address returnTo("return_to");
    assembler.lookupAddress(returnTo);

    const uint8_t sp = core.o_debug_s;
    const uint8_t p = C | D;

    // activate the NMI
    core.i_nmi_n = 0;
    testBench.tick(11);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(11)
        .port(o_rw).signal("11110001111").repeatEachStep(2)
        .port(o_sync).signal("10100000010").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(9)
            .signal({returnTo.hi()}).repeat(2)
            .signal({returnTo.lo()}).repeat(2)
            .signal({p}).repeat(2)
            .signal({0}).repeat(7)
        .port(o_address)
            .signal({
                // NOP - at 'start'
                start.byteIndex(),
                start.byteIndex() + 1u,

                // IRQ
                returnTo.byteIndex(),
                returnTo.byteIndex(),
                0x0100u + sp,
                0x0100u + sp - 1u,
                0x0100u + sp - 2u,
                0xFFFA,                     // Non Maskable Interrupt Vector (low byte)
                0xFFFB,                     // Non Maskable Interrupt Vector (high byte)

                // NOP (at interrupt vector)
                nonMaskableInterrupt.byteIndex(),
                nonMaskableInterrupt.byteIndex() + 1u
            }).repeatEachStep(2)
        .port(o_debug_s)
            .signal({sp}).repeat(7)
            .signal({sp - 3u}).repeat(4)
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    // processor flags should be augmented with I=Interrupt
    EXPECT_EQ(p | I, core.o_debug_p);
}

TEST_F(Cpu6502, ShouldNotIgnoreNMIWhenProcessorStatusHasI) {
    auto& core = testBench.core();

    sram.clear(0);

    Assembler assembler;
    assembler
            .NOP()
        .org(0x1234)
        .label("init")
            .SEC()                  // non-zero Processor Status (P)
            .SED()
            .SEI()                  // set the I flag for masking IRQ
        .label("start")
            .NOP()
        .label("return_to")
            .NOP()
            .NOP()
        .org(0x3456)
        .label("non_maskable_interrupt")
            .NOP()
        .org(0xFFFA)                // INTERRUPT VECTOR
        .word("non_maskable_interrupt")
        .org(0xFFFC)                // RESET VECTOR
        .word("init")
        .compileTo(sram);

    helperSkipResetVector();

    // skip init section
    testBench.tick(6);
    testBench.trace.clear();

    // capture information from the assembler / simulated core
    cpu6502::assembler::Address start("start");
    assembler.lookupAddress(start);

    cpu6502::assembler::Address nonMaskableInterrupt("non_maskable_interrupt");
    assembler.lookupAddress(nonMaskableInterrupt);

    cpu6502::assembler::Address returnTo("return_to");
    assembler.lookupAddress(returnTo);

    const uint8_t sp = core.o_debug_s;
    const uint8_t p = C | D | I;

    // activate the NMI
    core.i_nmi_n = 0;
    testBench.tick(11);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(11)
        .port(o_rw).signal("11110001111").repeatEachStep(2)
        .port(o_sync).signal("10100000010").repeatEachStep(2)
        .port(o_data)
            .signal({0}).repeat(9)
            .signal({returnTo.hi()}).repeat(2)
            .signal({returnTo.lo()}).repeat(2)
            .signal({p}).repeat(2)
            .signal({0}).repeat(7)
        .port(o_address)
            .signal({
                // NOP - at 'start'
                start.byteIndex(),
                start.byteIndex() + 1u,

                // IRQ
                returnTo.byteIndex(),
                returnTo.byteIndex(),
                0x0100u + sp,
                0x0100u + sp - 1u,
                0x0100u + sp - 2u,
                0xFFFA,                     // Non Maskable Interrupt Vector (low byte)
                0xFFFB,                     // Non Maskable Interrupt Vector (high byte)

                // NOP (at interrupt vector)
                nonMaskableInterrupt.byteIndex(),
                nonMaskableInterrupt.byteIndex() + 1u
            }).repeatEachStep(2)
        .port(o_debug_s)
            .signal({sp}).repeat(7)
            .signal({sp - 3u}).repeat(4)
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));

    // processor flags should be augmented with I=Interrupt
    EXPECT_EQ(p | I, core.o_debug_p);
}

// DONE: IRQ - should default simulation to 1
// DONE: should handle IRQ - i_irq_n = 0, set I flag during interrupt
// DONE: should ignore i_irq_n = 0, while I flag is set - then use it when the flag is cleared
// DONE: NMI - should default simulation to 1
// DONE: should handle NMI - i_nmi_n = 0 for 2 cycles after falling from 1 
// DONE: should not ignore i_nmi_n = 0, when I flag is set

// should not ingore i_irq_n = 0, when returning from interrupt handler
// should ignore i_nmi_n = 0 after returning from NMI, until it goes high+low again
// IRQ and NMI at the same time - prefer NMI, and then IRQ
