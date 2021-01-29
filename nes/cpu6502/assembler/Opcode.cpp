#include "Opcode.h"
#include "Assembler.h"

#include <cassert>

namespace cpu6502 { namespace assembler {
    Opcode::Opcode() : m_addressingMode(kImplied), m_byteIndex(0), m_assembler(0) {
    }

    Opcode::~Opcode() {
        
    }

    void Opcode::init(Assembler* assembler) {
        m_assembler = assembler;
    }
    
    Opcode& Opcode::immediate(uint8_t value) {
        m_addressingMode |= kImmediate;
        m_immediate = value;

        return *this;
    }

    Opcode& Opcode::a(const Address& address) {
        return absolute(address);
    }

    Opcode& Opcode::absolute(const Address& address) {
        m_addressingMode |= kAbsolute;
        m_absolute = address;

        return *this;
    }

    Opcode& Opcode::A() {
        m_addressingMode |= kAccumulator;

        return *this;
    }

    Opcode& Opcode::relative(const Address& address) {
        m_addressingMode |= kRelative;
        m_relative = address;

        return *this;
    }

    uint8_t Opcode::offset() const {
        uint8_t offset = uint8_t(m_relative.byteIndex() - (m_byteIndex + 2));

        return offset;
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

    bool Opcode::isRelative() const {
        return (m_addressingMode == kRelative);
    }

    void Opcode::setByteIndex(uint16_t byteIndex) {
        m_byteIndex = byteIndex;
    }

    void Opcode::registerAddresses() {
    }

    void Opcode::lookupAddresses() {
        m_assembler->lookupAddress(m_absolute);
        m_assembler->lookupAddress(m_relative);
    }

} // assembler
} // cpu6502
