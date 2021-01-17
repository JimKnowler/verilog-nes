#include "Opcode.h"

#include <cassert>

namespace cpu6502 { namespace assembler {
    Opcode::Opcode() : m_addressingMode(kImplied) {
    }

    Opcode::~Opcode() {
        
    }
    
    Opcode& Opcode::immediate(uint8_t value) {
        m_addressingMode |= kImmediate;
        m_immediate = value;

        return *this;
    }

    Opcode& Opcode::a(uint16_t value) {
        return absolute(value);
    }

    Opcode& Opcode::absolute(uint16_t value) {
        m_addressingMode |= kAbsolute;
        m_absolute = value;

        return *this;
    }

    Opcode& Opcode::A() {
        m_addressingMode |= kAccumulator;

        return *this;
    }

    std::vector<uint8_t> Opcode::serialise() const {
        assert(!"should be implemented by sub class");

        return std::vector<uint8_t>();
    }

    Opcode::operator uint8_t() const {
        return serialise()[0];
    }

    bool Opcode::isImplied() const {
        return (m_addressingMode == kImplied);
    }

    bool Opcode::isAccumulator() const {
        return (m_addressingMode == kAccumulator);
    }

    bool Opcode::isImmediate() const {
        return (m_addressingMode == kImmediate);
    }

    bool Opcode::isAbsolute() const {
        return (m_addressingMode == kAbsolute);
    }

} // assembler
} // cpu6502
