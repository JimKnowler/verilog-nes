#include "Assembler.hpp"
#include "Byte.hpp"

namespace cpu6502 { namespace assembler {
    Byte::Byte(uint8_t data) {
        m_data = data;
    }

    std::vector<uint8_t> Byte::serialise() const {
        std::vector<uint8_t> bytes;
        bytes.push_back(m_data);

        return bytes;        
    }
}}
