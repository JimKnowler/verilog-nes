#include "../Opcodes.h"

OPCODE_IMPL(INY, {
    addAddressingMode(kImplied, 0xC8);
});
