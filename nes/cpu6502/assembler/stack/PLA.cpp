#include "../Opcodes.hpp"

OPCODE_IMPL(PLA, {
    addAddressingMode(kImplied, 0x68);
});
