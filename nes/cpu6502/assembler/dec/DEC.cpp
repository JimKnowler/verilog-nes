#include "../Opcodes.h"

OPCODE_IMPL(DEC, {
    addAddressingMode(kAbsolute, 0xCE);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xDE);
});
