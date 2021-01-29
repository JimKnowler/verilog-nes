#include "Assembler.h"
#include "Label.h"

namespace cpu6502 { namespace assembler {
    Label::Label(const char* label) {
        m_label = label;
    }

    std::vector<uint8_t> Label::serialise() const {
        std::vector<uint8_t> emptyBytes;

        return emptyBytes;        
    }

    void Label::registerAddresses() {
        Opcode::registerAddresses();

        Address address(m_label.c_str(), m_byteIndex);
        m_assembler->registerAddress(address);
    }
}}
