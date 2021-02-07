#include "../Opcodes.h"

OPCODE_IMPL(RTI, {
    addAddressingMode(kImplied, 0x40);
})