#pragma once

#include "Opcode.hpp"

namespace cpu6502 {
    namespace assembler {
        /// @class Word
        /// @brief Assembler mneumonic for 16bits of raw data
        /// @param address raw data is implemented as an address, so we support string labels or numeric byte index
        class Word : public Opcode {
        public:
            Word(const Address& address);

            std::vector<uint8_t> serialise() const override;
        };
    }
}

