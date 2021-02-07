#include "../Opcodes.h"

OPCODE_IMPL(LDX, {
    addAddressingMode(kImmediate, 0xA2);
    addAddressingMode(kAbsolute, 0xAE);
});
