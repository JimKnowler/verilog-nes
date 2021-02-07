#include "../Opcodes.h"

OPCODE_IMPL(CLI, {
    addAddressingMode(kImplied, 0x58);
});
