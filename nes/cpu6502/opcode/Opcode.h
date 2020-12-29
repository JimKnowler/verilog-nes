#pragma once

#include <initializer_list>
#include <cstdint>

#define OPCODE_DEFN(_opcode) \
    class _opcode : public Opcode { \
        public: \
            std::initializer_list<uint8_t> serialise() const override; \
    }

#define OPCODE_IMPL(_opcode, _implementation) \
    namespace cpu6502 { namespace opcode { \
        std::initializer_list<uint8_t> _opcode::serialise() const \
        _implementation \
    }}

namespace cpu6502 {
    namespace opcode {
        /// @class Opcode
        /// @brief Base case for 6502 opcode assembler
        class Opcode {
        public:


            /// @brief Serialise the opcode to a byte stream
            /// @note Must be implemented in sub-classes
            virtual std::initializer_list<uint8_t> serialise() const = 0;
        };
    }
}