#include "Address.h"

namespace cpu6502 { namespace assembler {
    Address::Address() : m_byteIndex(0) {

    }

    Address::Address(const char* label) : m_label(label), m_byteIndex(0) {
    }

    Address::Address(const uint16_t byteIndex) : m_byteIndex(byteIndex) {

    }

    uint16_t Address::byteIndex() const {
        return m_byteIndex;
    }

    uint8_t Address::hi() const {
        return (m_byteIndex >> 8) & 0xFF;
    }

    uint8_t Address::lo() const {
        return m_byteIndex & 0xFF;
    }
} // assembler
} // cpu6502
