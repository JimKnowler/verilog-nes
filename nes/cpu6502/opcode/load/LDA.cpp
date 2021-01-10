#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_IMMEDIATE = 0xA9;
    const uint8_t OPCODE_ABSOLUTE = 0xAD;
}

OPCODE_IMPL(LDA, {
    assert(m_hasImmediate || m_hasAbsolute);

    std::vector<uint8_t> bytes;

    if (m_hasImmediate) {
        bytes.push_back(OPCODE_IMMEDIATE);
        bytes.push_back(m_immediate);
    } else if (m_hasAbsolute) {
        bytes.push_back(OPCODE_ABSOLUTE);
        uint8_t low = m_absolute & 0xff;
        bytes.push_back(low);
        uint8_t high = (m_absolute >> 8) & 0xff;
        bytes.push_back(high);
    }

    return bytes;
});
