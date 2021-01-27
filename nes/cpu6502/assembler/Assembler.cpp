#include <cassert>

#include "nes/cpu6502/assembler/Assembler.h"
#include "nes/cpu6502/assembler/Opcodes.h"

#define OPCODE_ASM_IMPL(_opcode) \
    Assembler& Assembler::_opcode() { \
        m_opcodes.push_back(std::make_unique<::cpu6502::assembler::_opcode>()); \
        m_opcodes.back()->init(this); \
        return *this; \
    }

namespace cpu6502 { namespace assembler {
    Assembler::Assembler() {
    }

#define OPCODE(_x) OPCODE_ASM_IMPL(_x)
# include "OpcodeList.inl"
#undef OPCODE

    Assembler& Assembler::immediate(uint8_t value) {
        currentOpcode()->immediate(value);

        return *this;
    }

    Assembler& Assembler::a(uint16_t value) {
        currentOpcode()->a(value);
        
        return *this;
    }

    Assembler& Assembler::absolute(uint16_t value) {
        currentOpcode()->absolute(value);

        return *this;
    }

    Assembler& Assembler::A() {
        currentOpcode()->A();

        return *this;
    }

    Assembler& Assembler::relative(uint8_t value) {
        currentOpcode()->relative(value);

        return *this;
    }

    void Assembler::compileTo(memory::SRAM& sram) {
        compileFirstPass();

        compileSecondPassTo(sram);
    }

    void Assembler::lookupAddress(Address& address) {
        if (address.m_label.empty()) {
            return;
        }

        address.m_byteIndex = m_addressLookup[address.m_label];
    }

    void Assembler::registerAddress(const Address& address) {
        assert(!address.m_label.empty());

        m_addressLookup[address.m_label] = address.m_byteIndex;
    }

    /// @brief 1st pass - position opcodes in memory & register labels
    void Assembler::compileFirstPass() {
        std::vector<uint8_t> temp;
    
        for (auto& opcode: m_opcodes) {
            size_t byteIndex = temp.size();
            opcode->setByteIndex(uint16_t(byteIndex));

            auto byteCode = opcode->serialise();
            temp.insert(temp.end(), byteCode.begin(), byteCode.end());
        }
    }

    /// @brief 2nd pass - compile to program
    void Assembler::compileSecondPassTo(memory::SRAM& sram) {
        std::vector<uint8_t> program;
           
        for (auto& opcode: m_opcodes) {
            auto byteCode = opcode->serialise();
            program.insert(program.end(), byteCode.begin(), byteCode.end());
        }

        sram.write(0, program);
    }

    Opcode* Assembler::currentOpcode() {
        assert(!m_opcodes.empty());

        auto& opcode = m_opcodes[m_opcodes.size() - 1];
        
        return opcode.get();
    }
} // assembler
} // Cpu6502
