#include "../Opcodes.hpp"

OPCODE_IMPL(SEI, {
    addAddressingMode(kImplied, 0x78);
});
