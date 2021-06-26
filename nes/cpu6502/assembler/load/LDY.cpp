#include "../Opcodes.hpp"

OPCODE_IMPL(LDY, {
    addAddressingMode(kImmediate, 0xA0);
    addAddressingMode(kAbsolute, 0xAC);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xBC);
    addAddressingMode(kZeroPage, 0xA4);
});
