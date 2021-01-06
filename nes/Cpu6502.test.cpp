#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/Cpu6502TestBench.h"
using namespace cpu6502testbench;

#include "cpu6502/opcode/Opcodes.h"
using namespace cpu6502::opcode;

#include "memory/SRAM.h"
using namespace memory;

#include "cpu6502/Assembler.h"
using namespace cpu6502;

namespace {
    class Cpu6502 : public ::testing::Test {
    public:
        Cpu6502() : sram(64 * 1024) {
        }

        void SetUp() override {
            testBench.setClockPolarity(1);
            sram.clear(0);

            // simulation at the end of a clock phase, before
            //   transition to other clock phase
            testBench.setCallbackSimulateCombinatorial([this]{
                auto& cpu = testBench.core();

                if (cpu.i_clk == 1) {
                    // clock: end of phi2
                    // R/W data is valid on the bus
                    if (cpu.o_rw == 0) {
                        // write
                        sram.write(cpu.o_address, cpu.o_data);
                    } else {
                        // read
                        cpu.i_data = sram.read(cpu.o_address);
                    }
                } else {
                    // clock: end of phi 1
                    // undefined data on the bus
                    cpu.i_data = 0xFF;
                }
            });

            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        /// @brief skip 7 steps of reset vector
        /// @note Next tick will be T0 of first opcode
        void helperSkipResetVector() {
            testBench.tick(7);
            testBench.trace.clear();
        }

        Cpu6502TestBench testBench;
        SRAM sram;
    };
}

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

TEST_F(Cpu6502, ShouldUseResetVector) {
    const uint8_t NOP = 0xEA;
    sram.clear(NOP);

    // write address for reset vector
    sram.write(0xFFFC, 0x02);               // low byte of 16bit address
    sram.write(0xFFFD, 0x80);               // high byte of 16bit address

    testBench.tick(8);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(8)
        .port(o_rw).signal("11").repeat(8)
        .port(o_sync).signal("01000000").repeatEachStep(2)
        .port(o_address)
            .signal({
                0x0000, 0x0001,             // PC, PC + 1
                0x01FF, 0x01FE, 0x01FD,     // SP, SP-1, SP-2
                0xFFFC, 0xFFFD,             // Reset Vector (low byte), Reset Vector (high byte)
                0x8002                      // The reset vector
            }).repeatEachStep(2);
        /*
        // TODO: would need to be zipped with random data during phi2
        .port(i_data)
            .signal({0})                    // memory simulation is warming up
            .signal({NOP}).repeat(5)        // reading unset memory
            .signal({0x02, 0x80})           // load PC from RESET vector
            .concat().repeatEachStep(2);
        */

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

#if 0

/// @note this test case is based on Ben Eater's video 
///       "'hello world' from scratch on a 6502 - Part 1"
///       https://www.youtube.com/watch?v=LnzuMJLZRdU
TEST_F(Cpu6502, ShouldExecuteNOP) {
    const uint8_t NOP = 0xEA;
    sram.clear(NOP);

    helperSkipResetVector();

    // test executing NOP opcodes
    // NOTE: NOP should execute for 2 clock ticks
    tick(5);
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(5)
        .port(o_rw).signal("11").repeat(5)
        .port(o_address).signal({
            0xEAEA, 0xEAEB,     // NOP: T0, T1
            0xEAEB, 0xEAEC,     // NOP: T0, T1
            0xEAEC}             // NOP: T0
        ).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldLDAi) {
    sram.clear(0);
    
    Assembler()
        .LDA().immediate(0x53)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    tick(5);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(5)
        .port(o_rw).signal("11")
                    .repeat(5)
        .port(o_address).signal({0, 1, 2, 3, 3})
                        .repeatEachStep(2)
        .port(o_debug_a).signal({0x00, 0x00, 0x53, 0x53, 0x53})
                        .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldLDAa) {
    sram.clear(0);
    
    Assembler()
        .LDA().a(0x5678)
        .NOP()
        .compileTo(sram);

    sram.write(0x5678, 0x42);

    helperSkipResetVector();

    tick(7);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(7)
        .port(o_rw).signal("11")
                    .repeat(7)
        .port(o_address).signal({0, 1, 2, 0x5678, 3, 4, 4})
                        .repeatEachStep(2)
        .port(o_debug_a).signal({0x00}).repeat(4)
                        .signal({0x42}).repeat(3)
                        .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

/// @note this test case is based on Ben Eater's video
///       "'hello world' from scratch on a 6502 - Part 2"
///       https://www.youtube.com/watch?v=yl8vPW5hydQ
TEST_F(Cpu6502, ShouldLDAiSTAa) {
    sram.clear(0);
    
    Assembler()
        .LDA().immediate(0x42)
        .STA().a(0x1234)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(9)
        .port(o_rw).signal("11").repeat(5)      // READ
                    .signal("00")               // WRITE
                    .signal("11").repeat(3)     // READ
        .port(o_data).signal({0, 0}).repeat(5)
                    .signal({0x42}).repeat(2)
                    .signal({0, 0}).repeat(3)
        .port(o_address).signal({0, 1, 2, 3, 4, 0x1234, 5, 6, 6 })
                        .repeatEachStep(2)
        .port(o_debug_a).signal({0x00}).repeat(2)
                        .signal({0x42}).repeat(7)
                        .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

#endif

// todo: test that o_sync is high during T1, especially on pipelined instructions