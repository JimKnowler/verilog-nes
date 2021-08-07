#include "../Opcodes.hpp"

OPCODE_IMPL(STA, {
    addAddressingMode(kAbsolute, 0x8D);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x9D);
    addAddressingMode(kAbsolute|kIndexedWithY, 0x99);
    addAddressingMode(kZeroPage, 0x85);
    addAddressingMode(kZeroPage|kIndirect|kIndexedWithY, 0x91);
    addAddressingMode(kZeroPage|kIndexedWithX, 0x95);
});
