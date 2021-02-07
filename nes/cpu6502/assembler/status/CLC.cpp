#include "../Opcodes.h"

OPCODE_IMPL(CLC, {
    addAddressingMode(kImplied, 0x18);
});
