#include "../Opcodes.hpp"

OPCODE_IMPL(RTI, {
    addAddressingMode(kImplied, 0x40);
})