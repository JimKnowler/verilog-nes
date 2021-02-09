#include <cassert>

#include "nes/cpu6502/assembler/Assembler.h"
#include "nes/cpu6502/assembler/Opcodes.h"
#include "nes/cpu6502/assembler/Label.h"
#include "nes/cpu6502/assembler/Org.h"
#include "nes/cpu6502/assembler/Word.h"
#include "nes/cpu6502/assembler/Byte.h"

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

    Assembler& Assembler::addOpcode(std::unique_ptr<Opcode>&& opcode) {
        m_opcodes.push_back(std::move(opcode));
        m_opcodes.back()->init(this);

        return *this;
    }

    Assembler& Assembler::label(const char* label) {
        addOpcode(std::make_unique<Label>(label));

        return *this;
    }

    Assembler& Assembler::org(uint16_t address) {
        addOpcode(std::make_unique<Org>(address));

        return *this;
    }

    Assembler& Assembler::word(const Address& address) {
        addOpcode(std::make_unique<Word>(address));

        return *this;
    }

    Assembler& Assembler::byte(uint8_t data) {
        addOpcode(std::make_unique<Byte>(data));

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

    Assembler& Assembler::indirect(const Address& address) {
        currentOpcode()->indirect(address);

        return *this;
    }
    
    Assembler& Assembler::x() {
        currentOpcode()->x();

        return *this;
    }

    Assembler& Assembler::y() {
        currentOpcode()->y();

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
    
        size_t byteIndex = 0;
        for (auto& opcode: m_opcodes) {
            byteIndex = opcode->setByteIndex(uint16_t(byteIndex));
            opcode->registerAddresses();

            auto byteCode = opcode->serialise();
            byteIndex += byteCode.size();
            temp.insert(temp.end(), byteCode.begin(), byteCode.end());
        }
    }

    /// @brief 2nd pass - compile to program
    void Assembler::compileSecondPassTo(memory::SRAM& sram) {
        std::vector<uint8_t> program;
        
        uint16_t byteIndex = 0;
        for (auto& opcode: m_opcodes) {
            uint16_t opcodeByteIndex = opcode->setByteIndex(byteIndex);
            if (opcodeByteIndex != byteIndex) {
                assert(opcodeByteIndex > byteIndex);

                if (byteIndex == 0) {
                    // using .org to place assembled code in a memory map
                    byteIndex = opcodeByteIndex;
                } else {
                    // using .org to position data at specific point in memory
                    uint16_t gapSize = opcodeByteIndex - byteIndex;
                    for (uint16_t i=0; i<gapSize; i++) {
                        program.push_back(0);
                    }

                    byteIndex = opcodeByteIndex;
                }
            }

            opcode->lookupAddresses();

            auto byteCode = opcode->serialise();
            byteIndex += uint16_t(byteCode.size());
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
