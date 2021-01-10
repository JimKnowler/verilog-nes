#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TXS, {
    assert(isImplied());

    std::vector<uint8_t> bytes = { 0x9A };

    return bytes;
});
