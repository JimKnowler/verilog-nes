#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TAX, {
    assert(isImplied());

    std::vector<uint8_t> bytes = { 0xAA };

    return bytes;
});
