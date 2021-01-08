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
    const uint8_t OPCODE_NOP = 0xEA;

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

        /// @brief skip T0-6 steps of reset vector
        /// @note Next tick will act as T0 of first opcode
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

TEST_F(Cpu6502, ShouldExecuteResetVector) {
    sram.clear(OPCODE_NOP);

    // write address for reset vector
    sram.write(0xFFFC, 0x02);               // low byte of 16bit address
    sram.write(0xFFFD, 0x80);               // high byte of 16bit address

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(9)
        .port(o_rw).signal("11").repeat(9)
        .port(o_sync).signal("010000001").repeatEachStep(2)
        .port(o_address)
            .signal({
                0x0000, 0x0001,             // PC, PC + 1
                0x01FF, 0x01FE, 0x01FD,     // SP, SP-1, SP-2
                0xFFFC, 0xFFFD,             // Reset Vector (low byte), Reset Vector (high byte)
                0x8002,                     // The reset vector (pointing at NOP)
                0x8003,                     // NOP incrememting PC
            }).repeatEachStep(2);
        /*
        // TODO: would need to be zipped with random data during phi2
        .port(i_data)
            .signal({0})                    // memory simulation is warming up
            .signal({OPCODE_NOP}).repeat(5)        // reading unset memory
            .signal({0x02, 0x80})           // load PC from RESET vector
            .signal({OPCODE_NOP})                  // load NOP instruction at RESET vector
            .concat().repeatEachStep(2);
        */

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

/// @note this test case is based on Ben Eater's video 
///       "'hello world' from scratch on a 6502 - Part 1"
///       https://www.youtube.com/watch?v=LnzuMJLZRdU
TEST_F(Cpu6502, ShouldExecuteNOP) {
    sram.clear(OPCODE_NOP);

    helperSkipResetVector();

    testBench.tick(5);
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(5)
        .port(o_rw).signal("11").repeat(5)
        .port(o_sync).signal("01010").repeatEachStep(2)
        .port(o_address).signal({
            0xEAEA, 0xEAEB,     // NOP: T0, T1
            0xEAEB, 0xEAEC,     // NOP: T0, T1
            0xEAEC}             // NOP: T0
        ).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldExecuteLDAi) {
    sram.clear(0);
    
    Assembler()
        .LDA().immediate(0x53)
        .NOP()
        .compileTo(sram);

    helperSkipResetVector();

    testBench.tick(4);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_rw).signal("11")
                    .repeat(4)
        .port(o_sync).signal("0101").repeatEachStep(2)
        .port(o_address).signal({0, 1, 2, 3})
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF, 0xFF, 0x53, 0x53})
                        .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldExecuteLDAa) {
    sram.clear(0);
    
    const uint16_t kTestAddress = 0x5678;
    const uint8_t kTestData = 0x42;

    Assembler()
        .LDA().a(kTestAddress)
        .NOP()
        .compileTo(sram);

    sram.write(kTestAddress, kTestData);

    helperSkipResetVector();

    testBench.tick(6);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(6)
        .port(o_rw).signal("11").repeat(6)
        .port(o_sync).signal("010001").repeatEachStep(2)
        .port(o_address)
            .signal({0, 1, 2, kTestAddress, 3, 4})
            .repeatEachStep(2)
        .port(o_debug_ac)
            .signal({0xFF}).repeat(4)
            .signal({kTestData}).repeat(2)
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldSTAa) {
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
        .port(o_sync).signal("010001").repeatEachStep(2)
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
