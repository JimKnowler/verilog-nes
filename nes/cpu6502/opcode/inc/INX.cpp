#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(INX, {
    assert(isImplied());

    std::vector<uint8_t> bytes = { 0xE8 };

    return bytes;
});
