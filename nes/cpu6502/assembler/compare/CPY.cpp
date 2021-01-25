#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_IMMEDIATE = 0xC0;
}

OPCODE_IMPL(CPY, {
    assert(isImmediate());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_IMMEDIATE);
    bytes.push_back(m_immediate);

    return bytes;
});