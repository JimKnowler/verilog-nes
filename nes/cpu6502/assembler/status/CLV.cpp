#include "../Opcodes.hpp"

OPCODE_IMPL(CLV, {
    addAddressingMode(kImplied, 0xB8); 
});
