#include "../Opcodes.h"

OPCODE_IMPL(CLV, {
    addAddressingMode(kImplied, 0xB8); 
});
