#include "../Opcodes.hpp"

OPCODE_IMPL(BNE, {
    addAddressingMode(kRelative, 0xD0);
});
