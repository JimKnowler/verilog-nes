#include "../Opcodes.h"

OPCODE_IMPL(LSR, {
    addAddressingMode(kAccumulator, 0x4A); 
});
