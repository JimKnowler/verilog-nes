#pragma once

#include "Opcode.h"

namespace cpu6502 {
    namespace assembler {
        class Label : public Opcode {
        public:
            Label(const char* label);

            std::vector<uint8_t> serialise() const override;

            virtual void registerAddresses() override;

        private:
            std::string m_label;
        };
    }
}

