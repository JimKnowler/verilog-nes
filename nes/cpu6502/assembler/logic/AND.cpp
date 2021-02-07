#include "../Opcodes.h"

OPCODE_IMPL(AND, {
    addAddressingMode(kImmediate, 0x29);
    addAddressingMode(kAbsolute, 0x2D);
});
