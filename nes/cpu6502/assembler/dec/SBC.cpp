#include "../Opcodes.hpp"

OPCODE_IMPL(SBC, {
    addAddressingMode(kImmediate, 0xE9);
    addAddressingMode(kAbsolute, 0xED);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xFD);
    addAddressingMode(kAbsolute|kIndexedWithY, 0xF9);
    addAddressingMode(kZeroPage|kIndexedWithX, 0xF5);
    addAddressingMode(kZeroPage, 0xE5);
    addAddressingMode(kZeroPage|kIndirect|kIndexedWithY, 0xF1);
    addAddressingMode(kZeroPage|kIndirect|kIndexedWithX, 0xE1);
});
