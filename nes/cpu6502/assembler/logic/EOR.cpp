#include "../Opcodes.h"

OPCODE_IMPL(EOR, {
    addAddressingMode(kImmediate, 0x49);
});
