#pragma once

enum ProcessorStatusFlags : uint8_t {
    C = 1 << 0,     // Carry
    Z = 1 << 1,     // Zero
    I = 1 << 2,     // Interrupt
    D = 1 << 3,     // Decimal Mode
    B = 1 << 4,     // Break
    U = 1 << 5,     // Unused - always reported as 1 by P
    V = 1 << 6,     // Overflow
    N = 1 << 7      // Negative
};
