#include "../Opcodes.hpp"

OPCODE_IMPL(BRK, {
    addAddressingMode(kImplied, 0x00);
})
