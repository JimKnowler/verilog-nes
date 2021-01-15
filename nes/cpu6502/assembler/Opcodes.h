#pragma once

#include "Opcode.h"

namespace cpu6502 {
    namespace assembler {
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
        OPCODE_DEFN(ASL);
        OPCODE_DEFN(ROL);
        OPCODE_DEFN(ROR);

        // status
        OPCODE_DEFN(CLC);
        OPCODE_DEFN(SEC);
        OPCODE_DEFN(SEI);
        OPCODE_DEFN(CLI);

        // stack
        OPCODE_DEFN(PHA);
        OPCODE_DEFN(PHP);
        OPCODE_DEFN(PLA);
        OPCODE_DEFN(PLP);
    }
}