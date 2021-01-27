#include "Opcodes.h"

OPCODE_IMPL(Label, {
    // todo: ask assembler for actual address for this label
    //  - on 1st pass, this will register 

    std::vector<uint8_t> emptyBytes;

    return emptyBytes;
});
