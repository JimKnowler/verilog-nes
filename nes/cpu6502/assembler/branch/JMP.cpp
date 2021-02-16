#include "../Opcodes.hpp"

OPCODE_IMPL(JMP, {
    addAddressingMode(kAbsolute, 0x4C); 
    addAddressingMode(kIndirect, 0x6C);
});
