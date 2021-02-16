#include "../Opcodes.hpp"

OPCODE_IMPL(LDA, {
    addAddressingMode(kImmediate, 0xA9);
    addAddressingMode(kAbsolute, 0xAD);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xBD);
    addAddressingMode(kAbsolute|kIndexedWithY, 0xB9);
})
