#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_ABSOLUTE = 0x8D;
}

OPCODE_IMPL(STA, {
    assert(isAbsolute());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_ABSOLUTE);
    uint8_t low = m_absolute & 0xff;
    bytes.push_back(low);
    uint8_t high = (m_absolute >> 8) & 0xff;
    bytes.push_back(high);

    return bytes;
});
