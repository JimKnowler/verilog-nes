#include "../Opcodes.h"

OPCODE_IMPL(ROL, {
    addAddressingMode(kAccumulator, 0x2A);
});
