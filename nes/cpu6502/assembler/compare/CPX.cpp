#include "../Opcodes.h"

OPCODE_IMPL(CPX, {
    addAddressingMode(kImmediate, 0xE0);
});
