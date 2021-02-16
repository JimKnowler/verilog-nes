#include "../Opcodes.hpp"

OPCODE_IMPL(DEY, {
    addAddressingMode(kImplied, 0x88);
});
