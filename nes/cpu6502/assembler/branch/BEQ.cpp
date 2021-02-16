#include "../Opcodes.hpp"

OPCODE_IMPL(BEQ, {
    addAddressingMode(kRelative, 0xF0);
});
