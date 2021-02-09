#include "../Opcodes.h"

OPCODE_IMPL(ROR, {
    addAddressingMode(kAccumulator, 0x6A);
    addAddressingMode(kAbsolute, 0x6E);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x7E);
});
