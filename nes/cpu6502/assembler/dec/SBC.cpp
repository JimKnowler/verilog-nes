#include "../Opcodes.h"

OPCODE_IMPL(SBC, {
    addAddressingMode(kImmediate, 0xE9);
});
