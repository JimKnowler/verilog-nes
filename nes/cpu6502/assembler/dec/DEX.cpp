#include "../Opcodes.h"

OPCODE_IMPL(DEX, {
    addAddressingMode(kImplied, 0xCA);
});
