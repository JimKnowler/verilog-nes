#include "../Opcodes.hpp"

#include <cassert>

OPCODE_IMPL(TXS, {
    addAddressingMode(kImplied, 0x9A);
});
