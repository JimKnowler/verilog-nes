#include "../Opcodes.hpp"

OPCODE_IMPL(CLD, {
    addAddressingMode(kImplied, 0xD8);
});
