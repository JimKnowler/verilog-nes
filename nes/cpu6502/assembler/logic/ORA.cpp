#include "../Opcodes.h"

OPCODE_IMPL(ORA, {
    addAddressingMode(kImmediate, 0x09);
});
