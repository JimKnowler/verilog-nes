#pragma once

#include "Opcode.hpp"

namespace cpu6502 {
    namespace assembler {
        /// @class Byte
        /// @brief Assembler mneumonic for 8bits of raw data
        /// @param data 8 bits of data to store in assembled output
        class Byte : public Opcode {
        public:
            Byte(uint8_t data);

            std::vector<uint8_t> serialise() const override;

        private:
            uint8_t m_data;
        };
    }
}

