#include "../Opcodes.hpp"

OPCODE_IMPL(CMP, {
    addAddressingMode(kImmediate, 0xC9);
    addAddressingMode(kAbsolute, 0xCD);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xDD);
    addAddressingMode(kAbsolute|kIndexedWithY, 0xD9);
    addAddressingMode(kZeroPage, 0xC5);
});
