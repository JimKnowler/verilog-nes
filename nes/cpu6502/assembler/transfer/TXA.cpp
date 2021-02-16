#include "../Opcodes.hpp"

#include <cassert>

OPCODE_IMPL(TXA, {
    addAddressingMode(kImplied, 0x8A);
});
