#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TYA, {
    assert(isImplied());

    std::vector<uint8_t> bytes = { 0x98 };

    return bytes;
});
