#include "../Opcodes.h"

OPCODE_IMPL(BIT, {
    addAddressingMode(kAbsolute, 0x2C);
});
