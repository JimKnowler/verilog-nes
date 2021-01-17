#include "Opcode.h"

#include <cassert>

namespace cpu6502 { namespace assembler {
    Opcode::Opcode() {
        m_hasImmediate = false;
        m_hasAbsolute = false;
        m_hasAccumulator = false;
    }

    Opcode::~Opcode() {
        
    }
    
    Opcode& Opcode::immediate(uint8_t value) {
        m_hasImmediate = true;
        m_immediate = value;

        return *this;
    }

    Opcode& Opcode::a(uint16_t value) {
        return absolute(value);
    }

    Opcode& Opcode::absolute(uint16_t value) {
        m_hasAbsolute = true;
        m_absolute = value;

        return *this;
    }

    Opcode& Opcode::A() {
        m_hasAccumulator = true;

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
        // todo: update after adding addressing modes
        return (!m_hasImmediate) && (!m_hasAbsolute) && (!m_hasAccumulator);
    }

    bool Opcode::isAccumulator() const {
        return m_hasAccumulator;
    }

    bool Opcode::isImmediate() const {
        return m_hasImmediate;
    }

} // assembler
} // cpu6502
