#include "../Opcodes.hpp"

OPCODE_IMPL(BVC, {
    addAddressingMode(kRelative, 0x50);
});
