#include "../Opcodes.hpp"

OPCODE_IMPL(BMI, {
    addAddressingMode(kRelative, 0x30);
});
