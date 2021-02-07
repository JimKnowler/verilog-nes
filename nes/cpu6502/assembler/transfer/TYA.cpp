#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TYA, {
    addAddressingMode(kImplied, 0x98);
});
