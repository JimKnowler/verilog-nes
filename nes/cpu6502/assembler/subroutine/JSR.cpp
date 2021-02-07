#include "../Opcodes.h"

OPCODE_IMPL(JSR, {
    addAddressingMode(kAbsolute, 0x20);
});
