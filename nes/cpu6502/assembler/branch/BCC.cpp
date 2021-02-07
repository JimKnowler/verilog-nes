#include "../Opcodes.h"

OPCODE_IMPL(BCC, {
    addAddressingMode(kRelative, 0x90);
});
