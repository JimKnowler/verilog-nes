#include "../Opcodes.hpp"

OPCODE_IMPL(SEC, {
    addAddressingMode(kImplied, 0x38);
});
