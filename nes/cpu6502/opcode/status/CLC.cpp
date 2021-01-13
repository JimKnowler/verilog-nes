#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_IMPLIED = 0x18;
}

OPCODE_IMPL(CLC, {
    assert(isImplied());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_IMPLIED);

    return bytes;
});
