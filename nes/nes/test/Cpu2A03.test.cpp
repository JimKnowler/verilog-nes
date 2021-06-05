#include <numeric>

#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/Cpu2A03TestBench.h"
using namespace cpu2a03testbench;

#include "nes/memory/SRAM.hpp"
using namespace memory;

#include "nes/cpu6502/assembler/Assembler.hpp"
using namespace cpu6502::assembler;

#include "nes/cpu6502/ProcessorStatusFlags.hpp"

namespace {
    class Cpu2A03 : public ::testing::Test {
    public:
        Cpu2A03(): sram(64 * 1024) {
            
        }

        void SetUp() override {
            testBench.setClockPolarity(1);
            
            auto& core = testBench.core();
            core.i_clk_en = 1;
            core.i_irq_n = 1;
            core.i_nmi_n = 1;
            sram.clear(0);

            // simulation at the end of a clock phase, before
            //   transition to other clock phase
            testBench.setCallbackSimulateCombinatorial([this, &core]{
                if (core.i_clk == 1) {
                    // clock: end of phi2
                    // R/W data is valid on the bus
                    if (core.o_rw == 0) {
                        // write
                        sram.write(core.o_address, core.o_data);
                    } else {
                        // read
                        core.i_data = sram.read(core.o_address);
                    }
                } else {
                    // clock: end of phi 1
                    // undefined data on the bus
                    core.i_data = 0xFF;
                }
            });

            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {

        }

        Cpu2A03TestBench testBench;
        SRAM sram;
    };
}

TEST_F(Cpu2A03, ShouldConstruct) {

}

TEST_F(Cpu2A03, ShouldImplementResetVector) {
    sram.clear(NOP());

    // write address for reset vector
    sram.write(0xFFFC, 0x12);               // low byte of 16bit address
    sram.write(0xFFFD, 0x80);               // high byte of 16bit address

    testBench.tick(9);

    Trace expected = TraceBuilder()
        .port(i_clk).signal("_-").repeat(9)
        .port(o_rw).signal("11").repeat(9)
        .port(o_sync).signal("100000010").repeatEachStep(2)
        .port(o_address)
            .signal({
                0x0000, 0x0000,             // PC, PC
                0x01FF, 0x01FE, 0x01FD,     // SP, SP-1, SP-2
                0xFFFC, 0xFFFD,             // Reset Vector (low byte), Reset Vector (high byte)
                0x8012,                     // The reset vector (pointing at NOP)
                0x8013,                     // NOP incrememting PC
            }).repeatEachStep(2)
        .port(o_debug_s)
            .signal({0xFF}).repeat(5)
            .signal({0xFC}).repeat(4)       // store SP-3 after 3rd push
            .concat().repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expected));
}