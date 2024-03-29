#include "../Opcodes.hpp"

OPCODE_IMPL(LDX, {
    addAddressingMode(kImmediate, 0xA2);
    addAddressingMode(kAbsolute, 0xAE);
    addAddressingMode(kAbsolute|kIndexedWithY, 0xBE);
    addAddressingMode(kZeroPage, 0xA6);
    addAddressingMode(kZeroPage|kIndexedWithY, 0xB6);
});
