#include <cassert>

#include "nes/cpu6502/assembler/Assembler.h"
#include "nes/cpu6502/assembler/Opcodes.h"

#define OPCODE_ASM_IMPL(_opcode) \
    Assembler& Assembler::_opcode() { \
        m_opcodes.push_back(std::make_unique<::cpu6502::assembler::_opcode>()); \
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

    void Assembler::compileTo(memory::SRAM& sram) {
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
