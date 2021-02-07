#include "Opcodes.h"

OPCODE_IMPL(NOP, {
    addAddressingMode(kImplied, 0xEA);
});
