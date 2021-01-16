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

    OPCODE_ASM_IMPL(NOP);
    OPCODE_ASM_IMPL(LDA);
    OPCODE_ASM_IMPL(LDX);
    OPCODE_ASM_IMPL(LDY);
    OPCODE_ASM_IMPL(STA);
    OPCODE_ASM_IMPL(DEX);
    OPCODE_ASM_IMPL(DEY);
    OPCODE_ASM_IMPL(INX);
    OPCODE_ASM_IMPL(INY);
    OPCODE_ASM_IMPL(TAX);
    OPCODE_ASM_IMPL(TAY);
    OPCODE_ASM_IMPL(TXA);
    OPCODE_ASM_IMPL(TYA);
    OPCODE_ASM_IMPL(TSX);
    OPCODE_ASM_IMPL(TXS);
    OPCODE_ASM_IMPL(LSR);
    OPCODE_ASM_IMPL(CLC);
    OPCODE_ASM_IMPL(SEC);
    OPCODE_ASM_IMPL(ASL);
    OPCODE_ASM_IMPL(SEI);
    OPCODE_ASM_IMPL(CLI);
    OPCODE_ASM_IMPL(CLV);
    OPCODE_ASM_IMPL(ROL);
    OPCODE_ASM_IMPL(ROR);
    OPCODE_ASM_IMPL(PHA);
    OPCODE_ASM_IMPL(PHP);
    OPCODE_ASM_IMPL(PLA);
    OPCODE_ASM_IMPL(PLP);

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
