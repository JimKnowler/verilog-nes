#include "../Opcodes.hpp"

OPCODE_IMPL(EOR, {
    addAddressingMode(kImmediate, 0x49);
    addAddressingMode(kAbsolute, 0x4D);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x5D);
    addAddressingMode(kAbsolute|kIndexedWithY, 0x59);
    addAddressingMode(kZeroPage, 0x45);
    addAddressingMode(kZeroPage|kIndexedWithX, 0x55);
});
