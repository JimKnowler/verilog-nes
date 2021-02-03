#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE = 0x40;
}

OPCODE_IMPL(RTI, {
    assert(isImplied());

    std::vector<uint8_t> bytes;

    bytes.push_back(OPCODE);
    
    return bytes;
})