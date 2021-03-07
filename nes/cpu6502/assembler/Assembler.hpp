#pragma once

#include <vector>
#include <memory>
#include <map>

#include "nes/memory/SRAM.hpp"
#include "nes/cpu6502/assembler/Opcodes.hpp"

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

            Assembler& addOpcode(std::unique_ptr<Opcode>&& opcode);

            Assembler& label(const char* label);
            Assembler& org(uint16_t address);
            Assembler& word(const Address& address);
            Assembler& byte(uint8_t data);

            Assembler& immediate(uint8_t value);  
            Assembler& a(const Address& address);
            Assembler& absolute(const Address& address); 
            Assembler& A();
            Assembler& relative(const Address& address);
            Assembler& indirect(const Address& address);
            Assembler& x();
            Assembler& y();
            Assembler& zp(const Address& address); 

            void compileTo(memory::SRAM& sram);

            void registerAddress(const Address& address);
            void lookupAddress(Address& address);
        private:
            Opcode* currentOpcode();
            void compileFirstPass();
            void compileSecondPassTo(memory::SRAM& sram);

            std::vector<std::unique_ptr<Opcode>> m_opcodes;
            std::map<const std::string, uint16_t> m_addressLookup;
        };
    }
}