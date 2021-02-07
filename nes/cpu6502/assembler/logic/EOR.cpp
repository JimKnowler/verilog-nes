#include "../Opcodes.h"

OPCODE_IMPL(EOR, {
    addAddressingMode(kImmediate, 0x49);
    addAddressingMode(kAbsolute, 0x4D);
});
