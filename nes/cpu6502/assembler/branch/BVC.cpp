#include "../Opcodes.h"

OPCODE_IMPL(BVC, {
    addAddressingMode(kRelative, 0x50);
});
