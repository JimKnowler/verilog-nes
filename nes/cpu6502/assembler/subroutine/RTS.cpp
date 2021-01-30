#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_IMPLIED = 0x60;
}

OPCODE_IMPL(RTS, {
    assert(isImplied());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_IMPLIED);

    return bytes;
});
