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

        void helperSkipResetVector() {
            /// @brief skip T0-6 steps of reset vector
            /// @note Next tick will act as T0 of first opcode
            testBench.tick(7);
            testBench.trace.clear();
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

TEST_F(Cpu2A03, ShouldImplementOAMDMA) {
    const uint32_t kOAMDMA = 0x4014;
    const uint32_t kOAMDATA = 0x2004;

    sram.clear(0);

    std::vector<uint8_t> dmaData;

    for (int i = 0; i<256; i++) {
        dmaData.push_back(i);
    }

    std::vector<uint32_t> dmaAddresses;
    for (int i=0; i<256; i++) {
        dmaAddresses.push_back( 0x3F00 + i );
    }

    for (int i = 0; i<256; i++) {
        sram.write( dmaAddresses[i], dmaData[i]);
    }

    Assembler assembler;
    assembler
        .label("lda")
            .LDA().immediate(0x3F)
        .label("sta")
            .STA().absolute(kOAMDMA)
        .label("nop")
            .NOP() 
        .compileTo(sram);

    cpu6502::assembler::Address lda("lda");
    assembler.lookupAddress(lda);

    cpu6502::assembler::Address sta("sta");
    assembler.lookupAddress(sta);

    cpu6502::assembler::Address nop("nop");
    assembler.lookupAddress(nop);
    
    helperSkipResetVector();
    testBench.tick(2);

    //
    // LDA 
    // 

    Trace expectedLoad = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(2)
        .port(o_rw).signal("11")
                    .repeat(2)
        .port(o_sync).signal("10").repeatEachStep(2)
        .port(o_address).signal({
                            lda.byteIndex(), 
                            lda.byteIndex() + 1u
                            })
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0xFF, 0xFF})
                        .repeatEachStep(2)
        .port(o_debug_x).signal({0x00}).repeat(4)
        .port(o_debug_y).signal({0x00}).repeat(4);

    EXPECT_THAT(expectedLoad, MatchesTrace(testBench.trace));
    
    //
    // STA
    // 

    testBench.trace.clear();
    testBench.tick(4);

    Trace expectedStore = TraceBuilder()
        .port(i_clk).signal("_-")
                    .repeat(4)
        .port(o_sync).signal("1000")
                     .repeatEachStep(2)
        .port(o_rw).signal("11").repeat(3)      // READ (STAa)
                    .signal("00")               // WRITE (STAa)
        .port(o_data).signal({0}).repeat(7)
                     .signal({0x3F})
        .port(o_address).signal({
                            sta.byteIndex(),
                            sta.byteIndex() + 1u,
                            sta.byteIndex() + 2u,
                            kOAMDMA
                            })          // STAa
                        .repeatEachStep(2)
        .port(o_debug_ac).signal({0x3F}).repeat(4)
                         .repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expectedStore));

    testBench.trace.clear();
    testBench.tick(513);

    //
    // OAM DMA
    // 

    TraceBuilder traceBuilderDMA;
    traceBuilderDMA
        .port(i_clk).signal("_-")
                    .repeat(513)
        .port(o_sync).signal("00")      // no op-codes loaded by CPU
                     .repeat(513)
        .port(o_rw).signal("11")       
                    .signal("1100").repeat(256)
        .port(o_address).signal({
                            0
                        })
                        .repeat(2);
    

    for (int i=0; i<256; i++) {
        traceBuilderDMA
            .signal({dmaAddresses[i], kOAMDATA})
            .repeatEachStep(2);
    }
                        
    traceBuilderDMA
        .port(o_data).signal({0}).repeat(2);

    for (int i=0; i<256; i++) {
        traceBuilderDMA.signal({0, dmaData[i]})
                        .repeatEachStep(2);
    }

    Trace expectedDMA = traceBuilderDMA;
    EXPECT_THAT(testBench.trace, MatchesTrace(expectedDMA));

    //
    // NOP (after DMA completes)
    //

    testBench.trace.clear();
    testBench.tick(2);

    Trace expectedNOP = TraceBuilder()
        .port(i_clk).signal("_-").repeat(2)
        .port(o_rw).signal("11").repeat(2)
        .port(o_sync).signal("10").repeatEachStep(2)
        .port(o_address).signal({
           nop.byteIndex(),
           nop.byteIndex() + 1u}
        ).repeatEachStep(2);

    EXPECT_THAT(testBench.trace, MatchesTrace(expectedNOP));
}


// should use i_clk_en to decide when DMA is running
// should use i_clk_en internally to disable CPU while DMA is running
// 