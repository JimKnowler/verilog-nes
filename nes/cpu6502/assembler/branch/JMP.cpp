#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_ABSOLUTE = 0x4C;
    const uint8_t OPCODE_INDIRECT = 0x6C;
}

OPCODE_IMPL(JMP, {
    assert(isAbsolute() || isIndirect());

    std::vector<uint8_t> bytes;

    if (isAbsolute()) {
        bytes.push_back(OPCODE_ABSOLUTE);
        bytes.push_back(m_absolute.lo());
        bytes.push_back(m_absolute.hi());
    } else {
        bytes.push_back(OPCODE_INDIRECT);
        bytes.push_back(m_indirect.lo());
        bytes.push_back(m_indirect.hi());
    }

    return bytes;
});
