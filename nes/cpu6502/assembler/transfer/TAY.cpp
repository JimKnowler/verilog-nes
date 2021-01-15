#include "../Opcodes.h"

#include <cassert>

OPCODE_IMPL(TAY, {
    assert(isImplied());

    std::vector<uint8_t> bytes = { 0xA8 };

    return bytes;
});
