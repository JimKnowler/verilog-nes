#include "../Opcodes.hpp"

OPCODE_IMPL(TAX, {
    addAddressingMode(kImplied, 0xAA);
});
