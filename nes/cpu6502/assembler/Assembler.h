#pragma once

#include <vector>
#include <memory>

#include "nes/memory/SRAM.h"
#include "nes/cpu6502/assembler/Opcodes.h"

#define OPCODE_ASM(_opcode) Assembler& _opcode();

namespace cpu6502 { 
    namespace assembler {
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
            OPCODE_ASM(ASL);
            OPCODE_ASM(SEI);
            OPCODE_ASM(CLI);
            OPCODE_ASM(CLV);
            OPCODE_ASM(ROL);
            OPCODE_ASM(ROR);
            OPCODE_ASM(PHA);
            OPCODE_ASM(PHP);
            OPCODE_ASM(PLA);
            OPCODE_ASM(PLP);

            Assembler& immediate(uint8_t value);  
            Assembler& a(uint16_t value);
            Assembler& absolute(uint16_t value); 
            Assembler& A();

            void compileTo(memory::SRAM& sram);
        
        private:
            Opcode* currentOpcode();

            std::vector<std::unique_ptr<Opcode>> m_opcodes;
        };
    }
}