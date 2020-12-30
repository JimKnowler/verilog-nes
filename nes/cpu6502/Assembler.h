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
        OPCODE_ASM(STA);
        OPCODE_ASM(NOP);

        Assembler& immediate(uint8_t value);  
        Assembler& a(uint16_t value);
        Assembler& absolute(uint16_t value); 

        void compileTo(memory::SRAM& sram);
    
    private:
        opcode::Opcode* currentOpcode();

        std::vector<std::unique_ptr<opcode::Opcode>> m_opcodes;
    };
}