#pragma once

#include "Opcode.h"

namespace cpu6502 {
    namespace assembler {
#define OPCODE(_x) OPCODE_DEFN(_x)
# include "OpcodeList.inl"
#undef OPCODE
    }
}
