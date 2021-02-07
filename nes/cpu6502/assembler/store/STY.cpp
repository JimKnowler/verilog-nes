#include "../Opcodes.h"

OPCODE_IMPL(STY, {
    addAddressingMode(kAbsolute, 0x8C);
});
