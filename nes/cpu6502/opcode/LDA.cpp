#include "Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_IMMEDIATE = 0xA9;
}

OPCODE_IMPL(LDA, {
    assert(m_hasImmediate);

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_IMMEDIATE);
    bytes.push_back(m_immediate);

    return bytes;
});
