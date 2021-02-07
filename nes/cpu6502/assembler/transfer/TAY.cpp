#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TAY, {
    addAddressingMode(kImplied, 0xA8);
});
