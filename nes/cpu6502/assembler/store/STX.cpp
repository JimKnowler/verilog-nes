#include "../Opcodes.h"

OPCODE_IMPL(STX, {
    addAddressingMode(kAbsolute, 0x8E);
});
