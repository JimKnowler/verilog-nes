#include "Opcode.h"

#include <cassert>

namespace cpu6502 { namespace opcode {
    Opcode::Opcode() {
        m_hasImmediate = false;
        m_hasAbsolute = false;
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

    std::vector<uint8_t> Opcode::serialise() const {
        assert(!"should be implemented by sub class");

        return std::vector<uint8_t>();
    }

    Opcode::operator uint8_t() const {
        return serialise()[0];
    }

} // opcode
} // cpu6502
