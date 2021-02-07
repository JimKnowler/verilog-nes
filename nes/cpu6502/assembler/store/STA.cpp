#include "../Opcodes.h"

OPCODE_IMPL(STA, {
    addAddressingMode(kAbsolute, 0x8D);
});
