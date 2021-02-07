#include "../Opcodes.h"

OPCODE_IMPL(BNE, {
    addAddressingMode(kRelative, 0xD0);
});
