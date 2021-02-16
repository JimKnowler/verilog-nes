#include "../Opcodes.hpp"


OPCODE_IMPL(BVS, {
    addAddressingMode(kRelative, 0x70);
});
