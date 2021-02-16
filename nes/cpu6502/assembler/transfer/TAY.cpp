#include "../Opcodes.hpp"

#include <cassert>

OPCODE_IMPL(TAY, {
    addAddressingMode(kImplied, 0xA8);
});
