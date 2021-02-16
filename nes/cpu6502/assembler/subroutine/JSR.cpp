#include "../Opcodes.hpp"

OPCODE_IMPL(JSR, {
    addAddressingMode(kAbsolute, 0x20);
});
