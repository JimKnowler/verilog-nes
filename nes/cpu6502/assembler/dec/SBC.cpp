#include "../Opcodes.h"

OPCODE_IMPL(SBC, {
    addAddressingMode(kImmediate, 0xE9);
    addAddressingMode(kAbsolute, 0xED);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xFD);
    addAddressingMode(kAbsolute|kIndexedWithY, 0xF9);
});
