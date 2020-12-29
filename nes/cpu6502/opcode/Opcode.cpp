#include "Opcode.h"

namespace cpu6502 { namespace opcode {
    Opcode::Opcode() {
        m_hasImmediate = false;
        m_hasAbsolute = false;
    }
            
    Opcode& Opcode::immediate(uint8_t value) {
        m_hasImmediate = true;
        m_immediate = value;

        return *this;
    }

    Opcode& Opcode::absolute(uint16_t value) {
        m_hasAbsolute = true;
        m_absolute = value;

        return *this;
    }


} // opcode
} // cpu6502
