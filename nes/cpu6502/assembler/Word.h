#pragma once

#include "Opcode.h"

namespace cpu6502 {
    namespace assembler {
        class Word : public Opcode {
        public:
            Word(const Address& address);

            std::vector<uint8_t> serialise() const override;

            void lookupAddresses() override;

        private:
            Address m_address;
        };
    }
}

