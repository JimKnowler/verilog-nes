#include "../Opcodes.hpp"

OPCODE_IMPL(CMP, {
    addAddressingMode(kImmediate, 0xC9);
    addAddressingMode(kAbsolute, 0xCD);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xDD);
    addAddressingMode(kAbsolute|kIndexedWithY, 0xD9);
    addAddressingMode(kZeroPage, 0xC5);
    addAddressingMode(kZeroPage|kIndexedWithX, 0xD5);
    addAddressingMode(kZeroPage|kIndirect|kIndexedWithY, 0xD1);
    addAddressingMode(kZeroPage|kIndirect|kIndexedWithX, 0xC1);
});
