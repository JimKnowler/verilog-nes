#include "../Opcodes.hpp"

OPCODE_IMPL(PLP, {
    addAddressingMode(kImplied, 0x28);
});
