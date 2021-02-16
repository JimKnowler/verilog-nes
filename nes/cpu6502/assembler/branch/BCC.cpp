#include "../Opcodes.hpp"

OPCODE_IMPL(BCC, {
    addAddressingMode(kRelative, 0x90);
});
