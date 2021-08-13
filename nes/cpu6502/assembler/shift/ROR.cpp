#include "../Opcodes.hpp"

OPCODE_IMPL(ROR, {
    addAddressingMode(kAccumulator, 0x6A);
    addAddressingMode(kAbsolute, 0x6E);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x7E);
    addAddressingMode(kZeroPage, 0x66);
    addAddressingMode(kZeroPage|kIndexedWithX, 0x76);
});
