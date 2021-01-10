#include "Opcodes.h"

#include <cassert>

OPCODE_IMPL(DEX, {
    assert(isImplied());

    std::vector<uint8_t> bytes = { 0xCA };

    return bytes;
});
