#include "Cpu6502.fixture.h"

// define CPU6502_VERBOSE to display CPU memory
//   simulation in TTY
//#define CPU6502_VERBOSE

Cpu6502::Cpu6502() : sram(64 * 1024) {
}

void Cpu6502::SetUp() {
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

#ifdef CPU6502_VERBOSE
            printf("Cpu6502: %s addr [0x%04x] data [0x%02x]\n", 
                    (cpu.o_rw == 1) ? "R" : "W",
                    cpu.o_address,
                    sram.read(cpu.o_address));
#endif
        } else {
            // clock: end of phi 1
            // undefined data on the bus
            //cpu.i_data = 0xFF;
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
