#pragma once

#include "Opcode.hpp"

namespace cpu6502 {
    namespace assembler {
        /// @class Org
        /// @brief Implement the .org assembler mneumonic for placing assembled code/data in memory
        /// @param address 16bit address 
        /// @note When .org is the first mneumonic (i.e. at byte index 0) then it is used by assembler to relocate 
        ///       all addresses for a memory mapped ROM/RAM
        /// @note When .org is used within other mneumonics (i.e. at byte index > 0) this it is used by the assembler
        ///       to insert padding bytes so that the following byte code is positioned as requested
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