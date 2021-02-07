#include "../Opcodes.h"

OPCODE_IMPL(SEC, {
    addAddressingMode(kImplied, 0x38);
});
