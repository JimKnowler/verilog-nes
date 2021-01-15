#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TXA, {
    assert(isImplied());

    std::vector<uint8_t> bytes = { 0x8A };

    return bytes;
});
