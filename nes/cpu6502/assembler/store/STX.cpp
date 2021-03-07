#include "../Opcodes.hpp"

OPCODE_IMPL(STX, {
    addAddressingMode(kAbsolute, 0x8E);
    addAddressingMode(kZeroPage, 0x86);
});
