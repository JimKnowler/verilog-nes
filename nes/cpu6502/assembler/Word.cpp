#include "Assembler.hpp"
#include "Word.hpp"

namespace cpu6502 { namespace assembler {
    Word::Word(const Address& address) {
        m_address = address;
    }

    std::vector<uint8_t> Word::serialise() const {
        std::vector<uint8_t> bytes;
        bytes.push_back(m_address.lo());
        bytes.push_back(m_address.hi());

        return bytes;        
    }
}}
