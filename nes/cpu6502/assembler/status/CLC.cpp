#include "../Opcodes.hpp"

OPCODE_IMPL(CLC, {
    addAddressingMode(kImplied, 0x18);
});
