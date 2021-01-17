#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_IMMEDIATE = 0x69;
}

OPCODE_IMPL(ADC, {
    assert(isImmediate());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_IMMEDIATE);

    return bytes;
});
