#include "../Opcodes.hpp"

#include <cassert>

OPCODE_IMPL(TYA, {
    addAddressingMode(kImplied, 0x98);
});
