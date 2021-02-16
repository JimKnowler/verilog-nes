#include "../Opcodes.hpp"

#include <cassert>

OPCODE_IMPL(TSX, {
    addAddressingMode(kImplied, 0xBA);
});
