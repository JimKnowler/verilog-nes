#pragma once

#include "Opcode.h"

namespace cpu6502 {
    namespace opcode {
        OPCODE_DEFN(NOP);
        
        // dec
        OPCODE_DEFN(DEX);
        OPCODE_DEFN(DEY);

        // inc
        OPCODE_DEFN(INX);
        OPCODE_DEFN(INY);
        
        // load
        OPCODE_DEFN(LDA);
        OPCODE_DEFN(LDX);
        OPCODE_DEFN(LDY);

        // store
        OPCODE_DEFN(STA);

        // transfer
        OPCODE_DEFN(TAX);
        OPCODE_DEFN(TAY);
        OPCODE_DEFN(TXA);
        OPCODE_DEFN(TYA);
        OPCODE_DEFN(TSX);
        OPCODE_DEFN(TXS);

        // shift
        OPCODE_DEFN(LSR);

        // status
        OPCODE_DEFN(CLC);
        OPCODE_DEFN(SEC);
    }
}
