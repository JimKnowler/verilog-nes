#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_RELATIVE = 0xB0;
}

OPCODE_IMPL(BCS, {
    assert(isRelative());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_RELATIVE);
    bytes.push_back(offset());

    return bytes;
});
