#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TXA, {
    addAddressingMode(kImplied, 0x8A);
});
