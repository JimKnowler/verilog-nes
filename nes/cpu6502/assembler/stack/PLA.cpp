#include "../Opcodes.h"

OPCODE_IMPL(PLA, {
    addAddressingMode(kImplied, 0x68);
});
