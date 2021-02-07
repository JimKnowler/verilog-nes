#include "../Opcodes.h"

OPCODE_IMPL(DEY, {
    addAddressingMode(kImplied, 0x88);
});
