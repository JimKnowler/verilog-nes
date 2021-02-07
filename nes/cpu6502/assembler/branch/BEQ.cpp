#include "../Opcodes.h"

OPCODE_IMPL(BEQ, {
    addAddressingMode(kRelative, 0xF0);
});
