#include "../Opcodes.hpp"

OPCODE_IMPL(INX, {
    addAddressingMode(kImplied, 0xE8);
});
