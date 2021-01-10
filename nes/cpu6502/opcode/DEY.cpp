#include "Opcodes.h"

#include <cassert>

OPCODE_IMPL(DEY, {
    assert(isImplied());

    std::vector<uint8_t> bytes = { 0x88 };

    return bytes;
});
