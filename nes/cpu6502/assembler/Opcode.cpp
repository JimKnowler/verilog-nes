#include "Opcode.hpp"
#include "Assembler.hpp"

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
        m_address = address;

        return *this;
    }

    Opcode& Opcode::A() {
        m_addressingMode |= kAccumulator;

        return *this;
    }

    Opcode& Opcode::relative(const Address& address) {
        m_addressingMode |= kRelative;
        m_address = address;

        return *this;
    }

    Opcode& Opcode::indirect(const Address& address) {
        m_addressingMode |= kIndirect;
        m_address = address;
        
        return *this;
    }

    Opcode& Opcode::x() {
        m_addressingMode |= kIndexedWithX;

        return *this;
    }

    Opcode& Opcode::y() {
        m_addressingMode |= kIndexedWithY;

        return *this;
    }

    uint8_t Opcode::offset() const {
        uint8_t offset = uint8_t(m_address.byteIndex() - (m_byteIndex + 2));

        return offset;
    }

    std::vector<uint8_t> Opcode::serialise() const {
        assert(supportsAddressingMode());

        auto it = m_addressingModes.find(m_addressingMode);
        uint8_t opcode = it->second;

        std::vector<uint8_t> bytes;
        bytes.push_back(opcode);

        switch (m_addressingMode) {
            case kImplied:
            case kAccumulator:
                break;
            case kImmediate:
                bytes.push_back(m_immediate);
                break;
            case kAbsolute:
            case kAbsolute|kIndexedWithX:
            case kAbsolute|kIndexedWithY:
            case kIndirect:
                bytes.push_back(m_address.lo());
                bytes.push_back(m_address.hi());
                break;
            case kRelative:
                bytes.push_back(offset());
                break;
            default:
                assert(!"unknown addressing mode");
        }

        return bytes;
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

    bool Opcode::isIndirect() const {
        return (m_addressingMode == kIndirect);
    }

    bool Opcode::supportsAddressingMode() const {
        return m_addressingModes.find(m_addressingMode) != m_addressingModes.end();
    }

    void Opcode::addAddressingMode(uint32_t addressingMode, uint8_t opcode) {
        m_addressingModes[addressingMode] = opcode;
    }

    uint16_t Opcode::setByteIndex(uint16_t byteIndex) {
        m_byteIndex = byteIndex;
        
        return m_byteIndex;
    }

    void Opcode::registerAddresses() {
    }

    void Opcode::lookupAddresses() {
        m_assembler->lookupAddress(m_address);
    }

} // assembler
} // cpu6502
