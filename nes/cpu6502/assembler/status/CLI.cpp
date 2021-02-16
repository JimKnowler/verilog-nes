#include "../Opcodes.hpp"

OPCODE_IMPL(CLI, {
    addAddressingMode(kImplied, 0x58);
});
