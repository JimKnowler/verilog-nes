#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TSX, {
    addAddressingMode(kImplied, 0xBA);
});
