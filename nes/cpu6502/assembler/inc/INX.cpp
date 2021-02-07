#include "../Opcodes.h"

OPCODE_IMPL(INX, {
    addAddressingMode(kImplied, 0xE8);
});
