#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_ABSOLUTE = 0x20;
}

OPCODE_IMPL(JSR, {
    assert(isAbsolute());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_ABSOLUTE);
    bytes.push_back(m_absolute.lo());
    bytes.push_back(m_absolute.hi());

    return bytes;
});
