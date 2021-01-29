#include <cassert>

#include "nes/cpu6502/assembler/Assembler.h"
#include "nes/cpu6502/assembler/Opcodes.h"
#include "nes/cpu6502/assembler/Label.h"

#define OPCODE_ASM_IMPL(_opcode) \
    Assembler& Assembler::_opcode() { \
        addOpcode(std::make_unique<::cpu6502::assembler::_opcode>()); \
        return *this; \
    }

namespace cpu6502 { namespace assembler {
    Assembler::Assembler() {
    }

#define OPCODE(_x) OPCODE_ASM_IMPL(_x)
# include "OpcodeList.inl"
#undef OPCODE

    Assembler& Assembler::label(const char* label) {
        addOpcode(std::make_unique<Label>(label));

        return *this;
    }

    Assembler& Assembler::immediate(uint8_t value) {
        currentOpcode()->immediate(value);

        return *this;
    }

    Assembler& Assembler::a(const Address& address) {
        currentOpcode()->a(address);
        
        return *this;
    }

    Assembler& Assembler::absolute(const Address& address) {
        currentOpcode()->absolute(address);

        return *this;
    }

    Assembler& Assembler::A() {
        currentOpcode()->A();

        return *this;
    }

    Assembler& Assembler::relative(const Address& address) {
        currentOpcode()->relative(address);

        return *this;
    }

    void Assembler::compileTo(memory::SRAM& sram) {
        compileFirstPass();

        compileSecondPassTo(sram);
    }

    void Assembler::lookupAddress(Address& address) {
        if (address.m_label.empty()) {
            // address does not have a label to be looked up, so its'
            // byteIndex should already be valid
            return;
        }

        address.m_byteIndex = m_addressLookup[address.m_label];
    }

    void Assembler::registerAddress(const Address& address) {
        if (address.m_label.empty()) {
            // address does not have a label to be looked up
            return;
        }

        m_addressLookup[address.m_label] = address.m_byteIndex;
    }

    /// @brief 1st pass - position opcodes in memory & register labels
    void Assembler::compileFirstPass() {
        std::vector<uint8_t> temp;
    
        for (auto& opcode: m_opcodes) {
            size_t byteIndex = temp.size();
            opcode->setByteIndex(uint16_t(byteIndex));
            opcode->registerAddresses();

            auto byteCode = opcode->serialise();
            temp.insert(temp.end(), byteCode.begin(), byteCode.end());
        }
    }

    /// @brief 2nd pass - compile to program
    void Assembler::compileSecondPassTo(memory::SRAM& sram) {
        std::vector<uint8_t> program;
           
        for (auto& opcode: m_opcodes) {
            opcode->lookupAddresses();
            auto byteCode = opcode->serialise();
            program.insert(program.end(), byteCode.begin(), byteCode.end());
        }

        sram.write(0, program);
    }

    void Assembler::addOpcode(std::unique_ptr<Opcode>&& opcode) {
        m_opcodes.push_back(std::move(opcode));
        m_opcodes.back()->init(this);
    }

    Opcode* Assembler::currentOpcode() {
        assert(!m_opcodes.empty());

        auto& opcode = m_opcodes[m_opcodes.size() - 1];
        
        return opcode.get();
    }
} // assembler
} // Cpu6502
