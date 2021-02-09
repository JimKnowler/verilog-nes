#include "../Opcodes.h"

OPCODE_IMPL(INC, {
    addAddressingMode(kAbsolute, 0xEE);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xFE);
});
