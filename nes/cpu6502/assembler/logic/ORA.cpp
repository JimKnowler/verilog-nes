#include "../Opcodes.hpp"

OPCODE_IMPL(ORA, {
    addAddressingMode(kImmediate, 0x09);
    addAddressingMode(kAbsolute, 0x0D);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x1D);
    addAddressingMode(kAbsolute|kIndexedWithY, 0x19);
    addAddressingMode(kZeroPage, 0x05);
});
