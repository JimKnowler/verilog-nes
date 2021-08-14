#include "../Opcodes.hpp"

OPCODE_IMPL(ADC, {
    addAddressingMode(kImmediate, 0x69);
    addAddressingMode(kAbsolute, 0x6D);
    addAddressingMode(kAbsolute|kIndexedWithX, 0x7D);
    addAddressingMode(kAbsolute|kIndexedWithY, 0x79);
    addAddressingMode(kZeroPage, 0x65);
    addAddressingMode(kZeroPage|kIndexedWithX, 0x75);
    addAddressingMode(kZeroPage|kIndirect|kIndexedWithY, 0x71);
    addAddressingMode(kZeroPage|kIndirect|kIndexedWithX, 0x61);
});
