#include "../Opcodes.hpp"

OPCODE_IMPL(PHA, {
    addAddressingMode(kImplied, 0x48);
});
