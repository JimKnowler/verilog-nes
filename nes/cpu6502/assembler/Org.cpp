#include "Assembler.hpp"
#include "Org.hpp"

namespace cpu6502 { namespace assembler {
    Org::Org(uint16_t address) {
        m_address = address;
    }

    std::vector<uint8_t> Org::serialise() const {
        std::vector<uint8_t> emptyBytes;

        return emptyBytes;        
    }

    uint16_t Org::setByteIndex(uint16_t byteIndex)  {
        return m_address;
    }

}}
