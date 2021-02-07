#include "../Opcodes.h"

OPCODE_IMPL(DEC, {
    addAddressingMode(kAbsolute, 0xCE);
});
