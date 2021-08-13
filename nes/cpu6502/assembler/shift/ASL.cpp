#include "../Opcodes.hpp"

OPCODE_IMPL(ASL, {
    addAddressingMode(kAccumulator, 0x0A);
    addAddressingMode(kAbsolute, 0x0E);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x1E);
    addAddressingMode(kZeroPage, 0x06);
    addAddressingMode(kZeroPage|kIndexedWithX, 0x16);
});
