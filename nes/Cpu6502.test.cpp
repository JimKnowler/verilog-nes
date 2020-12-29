#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/Cpu6502TestBench.h"
using namespace cpu6502testbench;

#include "cpu6502/opcode/Assembler.h"
using namespace cpu6502::opcode;

#include "memory/SRAM.h"
using namespace memory;

namespace {
    class Cpu6502 : public ::testing::Test {
    public:
        Cpu6502() : sram(64 * 1024) {

        }

        void SetUp() override {
            testBench.setClockPolarity(1);
            sram.clear(0);

            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

         /// @brief simulate memory access using address, data + RW lines
        void tick(uint32_t numTicks) {
            for (uint32_t i=0; i<numTicks; i++) {
                auto& cpu = testBench.core();

                for (uint32_t j=0; j<2; j++) {
                    testBench.step();

                    if (cpu.i_clk == 1) {
                        // simulating that all memory access will occur in second half-cycle (step)
                        // of each clock tick, when address is valid while i_clk is high
                        
                        // note: actual READ/WRITE will be registered on next falling edge of the clock

                        if (cpu.o_rw == 0) {
                            // write
                            sram.write(cpu.o_address, cpu.o_data);
                        } else {
                            // read
                            cpu.i_data = sram.read(cpu.o_address);
                        }
                    }
                }
            }
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

    // write contents of the reset vector
    sram.write(0xFFFC, 0x02);               // low byte of 16bit address
    sram.write(0xFFFD, 0x80);               // high byte of 16bit address

    tick(3);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(3)
        .port(o_rw).signal("11").repeat(3)
        .port(o_address).signal({0xFFFC, 0xFFFD, 0x8002}).repeatEachStep(2)
        .port(i_data).signal({0, 0x02, 0x80}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}

TEST_F(Cpu6502, ShouldExecuteNOP) {
    const uint8_t NOP = 0xEA;
    sram.clear(NOP);

    // skip past the reset vector
    tick(2);
    testBench.trace.clear();

    tick(5);
    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(5)
        .port(o_rw).signal("11").repeat(5)
        .port(o_address).signal({0xEAEA, 0xEAEB, 0xEAEB, 0xEAEC, 0xEAEC}).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}