#pragma once

#include <vector>
#include <cstdint>

#define OPCODE_DEFN(_opcode) \
    class _opcode : public Opcode { \
        public: \
            std::vector<uint8_t> serialise() const override; \
    }

#define OPCODE_IMPL(_opcode, _implementation) \
    namespace cpu6502 { namespace opcode { \
        std::vector<uint8_t> _opcode::serialise() const \
        _implementation \
    }}

namespace cpu6502 {
    namespace opcode {
        /// @class Opcode
        /// @brief Base case for 6502 opcode assembler
        class Opcode {
        public:
            Opcode();
            virtual ~Opcode();

            Opcode& immediate(uint8_t value);  

            Opcode& a(uint16_t value);
            Opcode& absolute(uint16_t value);      

            /// @brief Serialise the opcode to a byte stream
            /// @note Must be implemented in sub-classes
            virtual std::vector<uint8_t> serialise() const;

        protected:
            // todo: turn these bool flags into a bitmask
            //     -> useful when setting multiple fields
            bool m_hasImmediate;
            bool m_hasAbsolute;

            uint8_t m_immediate;
            uint16_t m_absolute;
        };
    }
}