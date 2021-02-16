#include "../Opcodes.hpp"

OPCODE_IMPL(LSR, {
    addAddressingMode(kAccumulator, 0x4A);
    addAddressingMode(kAbsolute, 0x4E);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x5E);
});
