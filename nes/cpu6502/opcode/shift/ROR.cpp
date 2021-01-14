#include "../Opcodes.h"

#include <cassert>

namespace {
    const uint8_t OPCODE_ACCUMULATOR = 0x6A;
}

OPCODE_IMPL(ROR, {
    assert(isAccumulator());

    std::vector<uint8_t> bytes;
    bytes.push_back(OPCODE_ACCUMULATOR);

    return bytes;
});
