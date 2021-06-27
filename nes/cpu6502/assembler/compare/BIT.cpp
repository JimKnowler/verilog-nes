#include "../Opcodes.hpp"

OPCODE_IMPL(BIT, {
    addAddressingMode(kAbsolute, 0x2C);
    addAddressingMode(kZeroPage, 0x24);
});
