#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_IMMEDIATE = 0x09;
}

OPCODE_IMPL(ORA, {
    assert(isImmediate());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_IMMEDIATE);

    return bytes;
});
