#include "../Opcodes.hpp"

OPCODE_IMPL(CPX, {
    addAddressingMode(kImmediate, 0xE0);
    addAddressingMode(kAbsolute, 0xEC);
});
