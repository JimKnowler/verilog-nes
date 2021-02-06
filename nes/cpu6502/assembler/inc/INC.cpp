#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_ABSOLUTE = 0xEE;
}

OPCODE_IMPL(INC, {
    assert(isAbsolute());

    std::vector<uint8_t> bytes;    
    bytes.push_back(OPCODE_ABSOLUTE);
    bytes.push_back(m_address.lo());
    bytes.push_back(m_address.hi());

    return bytes;
});
