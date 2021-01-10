#include "Opcodes.h"

#include <cassert>

OPCODE_IMPL(INY, {
    assert(isImplied());

    std::vector<uint8_t> bytes = { 0xC8 };

    return bytes;
});
