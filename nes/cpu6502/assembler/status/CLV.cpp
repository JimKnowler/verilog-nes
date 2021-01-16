#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_IMPLIED = 0xB8;
}

OPCODE_IMPL(CLV, {
    assert(isImplied());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_IMPLIED);

    return bytes;
});
