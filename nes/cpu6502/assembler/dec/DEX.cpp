#include "../Opcodes.hpp"

OPCODE_IMPL(DEX, {
    addAddressingMode(kImplied, 0xCA);
});
