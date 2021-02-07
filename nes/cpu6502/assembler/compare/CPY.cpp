#include "../Opcodes.h"

OPCODE_IMPL(CPY, {
    addAddressingMode(kImmediate, 0xC0);
});
