#include "../Opcodes.h"

OPCODE_IMPL(BMI, {
    addAddressingMode(kRelative, 0x30);
});
