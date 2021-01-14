#include <cassert>

#include "nes/cpu6502/Assembler.h"
#include "nes/cpu6502/opcode/Opcodes.h"

#define OPCODE_ASM_IMPL(_opcode) \
    Assembler& Assembler::_opcode() { \
        m_opcodes.push_back(std::make_unique<opcode::_opcode>()); \
        return *this; \
    }

namespace cpu6502 {
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

    opcode::Opcode* Assembler::currentOpcode() {
        assert(!m_opcodes.empty());

        auto& opcode = m_opcodes[m_opcodes.size() - 1];
        
        return opcode.get();
    }
}