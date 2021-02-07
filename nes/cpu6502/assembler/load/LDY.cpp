#include "../Opcodes.h"

OPCODE_IMPL(LDY, {
    addAddressingMode(kImmediate, 0xA0);
    addAddressingMode(kAbsolute, 0xAC);
});
