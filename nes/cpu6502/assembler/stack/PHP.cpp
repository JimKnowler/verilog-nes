#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_IMPLIED = 0x08;
}

OPCODE_IMPL(PHP, {
    assert(isImplied());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_IMPLIED);

    return bytes;
});