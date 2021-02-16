#include "../Opcodes.hpp"

OPCODE_IMPL(ADC, {
    addAddressingMode(kImmediate, 0x69);
    addAddressingMode(kAbsolute, 0x6D);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x7D);
    addAddressingMode(kAbsolute|kIndexedWithY, 0x79);
});
