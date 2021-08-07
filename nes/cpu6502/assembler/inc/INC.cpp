#include "../Opcodes.hpp"

OPCODE_IMPL(INC, {
    addAddressingMode(kAbsolute, 0xEE);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xFE);
    addAddressingMode(kZeroPage, 0xE6);
    addAddressingMode(kZeroPage|kIndexedWithX, 0xF6);
});
