#include "../Opcodes.h"

OPCODE_IMPL(RTS, {
    addAddressingMode(kImplied, 0x60);
});
