#include "../Opcodes.h"

OPCODE_IMPL(TAX, {
    addAddressingMode(kImplied, 0xAA);
});
