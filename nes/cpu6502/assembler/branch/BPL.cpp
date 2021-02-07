#include "../Opcodes.h"

OPCODE_IMPL(BPL, {
    addAddressingMode(kRelative, 0x10);
});
