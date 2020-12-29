#include "Opcodes.h"

OPCODE_IMPL(NOP, {
    std::vector<uint8_t> bytes = {0xEA};

    return bytes;
});
