#include "../Opcodes.hpp"

OPCODE_IMPL(STY, {
    addAddressingMode(kAbsolute, 0x8C);
});
