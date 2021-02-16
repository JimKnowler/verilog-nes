#include "../Opcodes.hpp"

OPCODE_IMPL(BCS, {
    addAddressingMode(kRelative, 0xB0);
});
