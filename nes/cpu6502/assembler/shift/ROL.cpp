#include "../Opcodes.hpp"

OPCODE_IMPL(ROL, {
    addAddressingMode(kAccumulator, 0x2A);
    addAddressingMode(kAbsolute, 0x2E);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x3E);
});
