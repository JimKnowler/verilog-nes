#pragma once

#include <vector>
#include <cstdint>
#include <map>

#include "Address.h"

#define OPCODE_DEFN(_opcode) \
    class _opcode : public Opcode { \
        public: \
            _opcode(); \
    }

#define OPCODE_IMPL(_opcode, _implementation) \
    namespace cpu6502 { namespace assembler { \
        _opcode::_opcode() \
        _implementation \
    }}

namespace cpu6502 {
    namespace assembler {
        /// @brief forward declaration
        class Assembler;

        /// @class Opcode
        /// @brief Base case for 6502 opcode assembler
        class Opcode {
        public:
            Opcode();
            virtual ~Opcode();

            void init(Assembler* assembler);

            /// @brief Immediate operand
            Opcode& immediate(uint8_t value);

            /// @brief Absolute operand
            Opcode& a(const Address& address);
            Opcode& absolute(const Address& address);

            /// @brief Accumulator operand
            Opcode& A(); 

            /// @brief Relative operand
            Opcode& relative(const Address& address);

            /// @brief Indirect operand
            Opcode& indirect(const Address& indirect);

            /// @brief retrieve relative address as offset to opcode's location in memory
            uint8_t offset() const;

            /// @brief Serialise the opcode to a byte stream
            virtual std::vector<uint8_t> serialise() const;

            // cast to uint8_t opcode
            operator uint8_t() const;

            // set the position of the opcode in memory
            /// @return the byte index where the opcode expects to be placed
            virtual uint16_t setByteIndex(uint16_t byteIndex);

            // used by Assembler to register addresses that other 
            virtual void registerAddresses();

            // used by Assembler to lookup addresses that Opcode may require
            virtual void lookupAddresses();

        protected:
            enum AddressingMode : uint32_t {
                kImplied = 0,
                kImmediate = 1 << 0,
                kAbsolute = 1 << 1,
                kAccumulator = 1 << 2,
                kRelative = 1 << 3,
                kIndirect = 1 << 4
            };

            typedef std::map<uint32_t, uint8_t> AddressingModes;

            bool isImplied() const;
            bool isAccumulator() const;
            bool isImmediate() const;
            bool isAbsolute() const;
            bool isRelative() const;
            bool isIndirect() const;

            bool supportsAddressingMode() const;
            void addAddressingMode(uint32_t addressingMode, uint8_t opcode);

            uint32_t m_addressingMode;

            AddressingModes m_addressingModes;

            uint8_t m_immediate;
            Address m_address;

            // position of this opcode in SRAM
            uint16_t m_byteIndex;

            Assembler* m_assembler;
        };
    }
}