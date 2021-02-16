#include "Opcodes.hpp"

OPCODE_IMPL(NOP, {
    addAddressingMode(kImplied, 0xEA);
});
