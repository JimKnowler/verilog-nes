#include "../Opcodes.hpp"

OPCODE_IMPL(STA, {
    addAddressingMode(kAbsolute, 0x8D);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x9D);
    addAddressingMode(kAbsolute|kIndexedWithY, 0x99);
});
