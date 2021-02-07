#include "../Opcodes.h"

OPCODE_IMPL(ASL, {
    addAddressingMode(kAccumulator, 0x0A); 
});
