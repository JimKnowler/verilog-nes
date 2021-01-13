#pragma once

#include <vector>
#include <memory>

#include "nes/memory/SRAM.h"
#include "nes/cpu6502/opcode/Opcode.h"

#define OPCODE_ASM(_opcode) Assembler& _opcode();

namespace cpu6502 {
    /// @class Assembler
    /// @brief simple 6502 assembler for creating test cases
    class Assembler {
    public:
        Assembler();

        OPCODE_ASM(LDA);
        OPCODE_ASM(LDX);
        OPCODE_ASM(LDY);
        OPCODE_ASM(STA);
        OPCODE_ASM(NOP);
        OPCODE_ASM(DEX);
        OPCODE_ASM(DEY);
        OPCODE_ASM(INX);
        OPCODE_ASM(INY);
        OPCODE_ASM(TAX);
        OPCODE_ASM(TAY);
        OPCODE_ASM(TXA);
        OPCODE_ASM(TYA);
        OPCODE_ASM(TSX);
        OPCODE_ASM(TXS);
        OPCODE_ASM(LSR);
        OPCODE_ASM(SEC);
        OPCODE_ASM(CLC);

        Assembler& immediate(uint8_t value);  
        Assembler& a(uint16_t value);
        Assembler& absolute(uint16_t value); 
        Assembler& A();

        void compileTo(memory::SRAM& sram);
    
    private:
        opcode::Opcode* currentOpcode();

        std::vector<std::unique_ptr<opcode::Opcode>> m_opcodes;
    };
}