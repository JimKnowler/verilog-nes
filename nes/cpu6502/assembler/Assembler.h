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

#define OPCODE(_x) OPCODE_ASM(_x)
# include "OpcodeList.inl"
#undef OPCODE
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