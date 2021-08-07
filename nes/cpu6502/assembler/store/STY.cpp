#include "../Opcodes.hpp"

OPCODE_IMPL(STY, {
    addAddressingMode(kAbsolute, 0x8C);
    addAddressingMode(kZeroPage, 0x84);
    addAddressingMode(kZeroPage|kIndexedWithX, 0x94);
});
