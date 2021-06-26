#include "../Opcodes.hpp"

OPCODE_IMPL(LDA, {
    addAddressingMode(kImmediate, 0xA9);
    addAddressingMode(kAbsolute, 0xAD);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xBD);
    addAddressingMode(kAbsolute|kIndexedWithY, 0xB9);
    addAddressingMode(kZeroPage|kIndexedWithX|kIndirect, 0xA1);
    addAddressingMode(kZeroPage|kIndirect|kIndexedWithY, 0xB1);
    addAddressingMode(kZeroPage, 0xA5);
})
