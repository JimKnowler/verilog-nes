#include "Cpu6502.fixture.hpp"

// define CPU6502_VERBOSE to display CPU memory
//   simulation in TTY
//#define CPU6502_VERBOSE

Cpu6502::Cpu6502() : sram(64 * 1024) {
}

void Cpu6502::SetUp() {
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

#ifdef CPU6502_VERBOSE
            printf("Cpu6502: %s addr [0x%04x] data [0x%02x]\n", 
                    (core.o_rw == 1) ? "R" : "W",
                    core.o_address,
                    sram.read(core.o_address));
#endif
        } else {
            // clock: end of phi 1
            // undefined data on the bus
            core.i_data = 0xFF;
        }
    });

    testBench.reset();
    testBench.trace.clear();
}

void Cpu6502::TearDown() {
}

void Cpu6502::helperSkipResetVector() {
    /// @brief skip T0-6 steps of reset vector
    /// @note Next tick will act as T0 of first opcode
    testBench.tick(7);
    testBench.trace.clear();
}
