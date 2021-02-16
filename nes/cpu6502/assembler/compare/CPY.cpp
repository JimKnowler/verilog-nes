#include "../Opcodes.hpp"

OPCODE_IMPL(CPY, {
    addAddressingMode(kImmediate, 0xC0);
    addAddressingMode(kAbsolute, 0xCC);
});
