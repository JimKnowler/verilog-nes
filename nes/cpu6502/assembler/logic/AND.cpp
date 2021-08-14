#include "../Opcodes.hpp"

OPCODE_IMPL(AND, {
    addAddressingMode(kImmediate, 0x29);
    addAddressingMode(kAbsolute, 0x2D);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x3D);
    addAddressingMode(kAbsolute|kIndexedWithY, 0x39);
    addAddressingMode(kZeroPage, 0x25);
    addAddressingMode(kZeroPage|kIndexedWithX, 0x35);
    addAddressingMode(kZeroPage|kIndirect|kIndexedWithY, 0x31);
});
