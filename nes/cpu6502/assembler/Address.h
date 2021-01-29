#pragma once

#include <cstdint>
#include <string>

namespace cpu6502 {
    namespace assembler {
        /// @brief forward declaration
        class Assembler;

        /// @class Address
        /// @brief Abstraction of an address in assembly code (as label or byte index into sram)
        class Address {
        public:
            Address();
            Address(const char* label);
            Address(const uint16_t byteIndex);
            Address(const char* label, const uint16_t byteIndex);

            // retrieve address as 16bit byte index
            uint16_t byteIndex() const;

            // retrieve high byte of 16bit byte index
            uint8_t hi() const;

            // retrieve low byte of 16bit byte index
            uint8_t lo() const;

        private:
            friend class Assembler;

            std::string m_label;
            uint16_t m_byteIndex;
        };
    }
}