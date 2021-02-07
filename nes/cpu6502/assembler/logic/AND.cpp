#include "../Opcodes.h"

OPCODE_IMPL(AND, {
    addAddressingMode(kImmediate, 0x29);
});
