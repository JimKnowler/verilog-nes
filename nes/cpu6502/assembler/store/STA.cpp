#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_ABSOLUTE = 0x8D;
}

OPCODE_IMPL(STA, {
    assert(isAbsolute());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_ABSOLUTE);
    bytes.push_back(absoluteLowByte());
    bytes.push_back(absoluteHighByte());

    return bytes;
});
