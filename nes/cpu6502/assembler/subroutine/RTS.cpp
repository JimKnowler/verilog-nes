#include "../Opcodes.hpp"

OPCODE_IMPL(RTS, {
    addAddressingMode(kImplied, 0x60);
});
