#include "../Opcodes.h"

OPCODE_IMPL(CMP, {
    addAddressingMode(kImmediate, 0xC9);
});
