#include "../Opcodes.hpp"

OPCODE_IMPL(DEC, {
    addAddressingMode(kAbsolute, 0xCE);
    addAddressingMode(kAbsolute|kIndexedWithX, 0xDE);
});
