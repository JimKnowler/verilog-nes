#include "Assembler.h"

OPCODE_IMPL(NOP, {
    std::initializer_list<uint8_t> bytes;

    bytes = {0xEA};

    return bytes;
});
