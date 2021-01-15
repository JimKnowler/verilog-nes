#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TSX, {
    assert(isImplied());

    std::vector<uint8_t> bytes = { 0xBA };

    return bytes;
});
