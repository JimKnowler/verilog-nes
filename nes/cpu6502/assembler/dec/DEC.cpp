#include "../Opcodes.hpp"

OPCODE_IMPL(DEC, {
    addAddressingMode(kAbsolute, 0xCE);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xDE);
    addAddressingMode(kZeroPage, 0xC6);
    addAddressingMode(kZeroPage|kIndexedWithX, 0xD6);
});
