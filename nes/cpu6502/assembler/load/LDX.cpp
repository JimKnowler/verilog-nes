#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_IMMEDIATE = 0xA2;
    const uint8_t OPCODE_ABSOLUTE = 0xAE;
}

OPCODE_IMPL(LDX, {
    assert(isImmediate() || isAbsolute());

    std::vector<uint8_t> bytes;

    if (isImmediate()) {
        bytes.push_back(OPCODE_IMMEDIATE);
        bytes.push_back(m_immediate);
    } else if (isAbsolute()) {
        bytes.push_back(OPCODE_ABSOLUTE);
        bytes.push_back(m_address.lo());
        bytes.push_back(m_address.hi());
    }

    return bytes;
});
