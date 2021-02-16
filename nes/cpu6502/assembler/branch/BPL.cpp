#include "../Opcodes.hpp"

OPCODE_IMPL(BPL, {
    addAddressingMode(kRelative, 0x10);
});
