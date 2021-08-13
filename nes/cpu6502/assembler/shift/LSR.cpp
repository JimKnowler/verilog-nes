#include "../Opcodes.hpp"

OPCODE_IMPL(LSR, {
    addAddressingMode(kAccumulator, 0x4A);
    addAddressingMode(kAbsolute, 0x4E);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x5E);
    addAddressingMode(kZeroPage, 0x46);
    addAddressingMode(kZeroPage|kIndexedWithX, 0x56);
});
