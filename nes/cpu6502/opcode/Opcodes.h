#pragma once

#include "Opcode.h"

namespace cpu6502 {
    namespace opcode {
        OPCODE_DEFN(LDA);
        OPCODE_DEFN(LDX);
        OPCODE_DEFN(LDY);
        OPCODE_DEFN(STA);
        OPCODE_DEFN(NOP);
    }
}
