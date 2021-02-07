#include "../Opcodes.h"

OPCODE_IMPL(SEI, {
    addAddressingMode(kImplied, 0x78);
});
