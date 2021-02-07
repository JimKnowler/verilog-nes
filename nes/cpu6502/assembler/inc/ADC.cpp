#include "../Opcodes.h"

OPCODE_IMPL(ADC, {
    addAddressingMode(kImmediate, 0x69);
});
