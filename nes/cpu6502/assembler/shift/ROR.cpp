#include "../Opcodes.h"

OPCODE_IMPL(ROR, {
    addAddressingMode(kAccumulator, 0x6A);
});
