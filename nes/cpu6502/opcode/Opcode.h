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

            /// @brief Immediate operand
            Opcode& immediate(uint8_t value = 0);  

            /// @brief Absolute operand
            Opcode& a(uint16_t value = 0);
            Opcode& absolute(uint16_t value = 0);

            /// @brief Accumulator operand
            Opcode& A(); 

            /// @brief Serialise the opcode to a byte stream
            /// @note Must be implemented in sub-classes
            virtual std::vector<uint8_t> serialise() const;

            // cast to uint8_t opcode
            operator uint8_t() const;

        protected:
            bool isImplied() const;
            bool isAccumulator() const;

            // todo: turn these bool flags into a bitmask
            //     -> useful when setting multiple fields
            bool m_hasImmediate;
            bool m_hasAbsolute;
            bool m_hasAccumulator;

            uint8_t m_immediate;
            uint16_t m_absolute;
        };
    }
}