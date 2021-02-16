#include "../Opcodes.hpp"

OPCODE_IMPL(INY, {
    addAddressingMode(kImplied, 0xC8);
});
