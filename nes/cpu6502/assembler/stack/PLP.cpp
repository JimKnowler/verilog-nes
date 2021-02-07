#include "../Opcodes.h"

OPCODE_IMPL(PLP, {
    addAddressingMode(kImplied, 0x28);
});
