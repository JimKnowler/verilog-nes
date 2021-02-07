#include "../Opcodes.h"

OPCODE_IMPL(PHA, {
    addAddressingMode(kImplied, 0x48);
});
