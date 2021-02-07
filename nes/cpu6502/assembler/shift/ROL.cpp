#include "../Opcodes.h"

OPCODE_IMPL(ROL, {
    addAddressingMode(kAccumulator, 0x2A);
    addAddressingMode(kAbsolute, 0x2E);
});
