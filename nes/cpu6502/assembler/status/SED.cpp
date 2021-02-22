#include "../Opcodes.hpp"

OPCODE_IMPL(SED, {
    addAddressingMode(kImplied, 0xF8);
});
