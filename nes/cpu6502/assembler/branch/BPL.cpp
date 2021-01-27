#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_RELATIVE = 0x10;
}

OPCODE_IMPL(BPL, {
    assert(isRelative());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_RELATIVE);
    bytes.push_back(offset());

    return bytes;
});
