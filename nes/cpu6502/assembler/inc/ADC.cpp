#include "../Opcodes.h"

OPCODE_IMPL(ADC, {
    addAddressingMode(kImmediate, 0x69);
    addAddressingMode(kAbsolute, 0x6D);
});
