#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TXS, {
    addAddressingMode(kImplied, 0x9A);
});
