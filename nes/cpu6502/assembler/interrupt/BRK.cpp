#include "../Opcodes.h"

OPCODE_IMPL(BRK, {
    addAddressingMode(kImplied, 0x00);
})
