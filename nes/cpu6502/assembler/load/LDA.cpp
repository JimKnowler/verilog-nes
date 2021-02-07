#include "../Opcodes.h"

OPCODE_IMPL(LDA, {
    addAddressingMode(kImmediate, 0xA9);
    addAddressingMode(kAbsolute, 0xAD);
})
