#pragma once

#include "Opcode.h"

namespace cpu6502 {
    namespace assembler {
        class Org : public Opcode {
        public:
            Org(uint16_t address);

            std::vector<uint8_t> serialise() const override;

            uint16_t setByteIndex(uint16_t byteIndex) override;

        private:
            uint16_t m_address;
        };
    }
}