#include "../Opcodes.h"


OPCODE_IMPL(BVS, {
    addAddressingMode(kRelative, 0x70);
});
