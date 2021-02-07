#include "../Opcodes.h"

OPCODE_IMPL(BCS, {
    addAddressingMode(kRelative, 0xB0);
});
