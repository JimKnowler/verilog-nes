#pragma once 

#include "nes/memory/SRAM.hpp"
#include "nes/cpu6502/assembler/Opcode.hpp"

namespace cpu6502 {
    namespace assembler {

        class Disassembler {
        public:
            Disassembler();

            struct DisassembledOpcode {
                uint8_t opcode;
                uint32_t addressingMode;
                std::string labelOpcode;
                std::string labelOperands;
                uint16_t pc;
                uint16_t byteSize;
                std::vector<uint8_t> data;
            };

            typedef std::vector<DisassembledOpcode> DisassembledOpcodes;

            DisassembledOpcodes disassemble(const memory::SRAM& sram, uint16_t pc, int numOpcodes);

        private:
            struct OpcodeInfo {
                std::string label;
                uint32_t    addressingMode;
            };

            std::map<uint8_t, OpcodeInfo> opcodes;

            void registerOpcodes();
            void registerOpcode(const std::string& label, const Opcode* opcode);

            struct ParseOpcodeInfo {
                uint16_t pc;
                const memory::SRAM& sram;
            };

            bool parseOpcodeAbsolute(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeAbsoluteIndexedIndirect(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeAbsoluteIndexedWithX(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeAbsoluteIndexedWithY(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeAbsoluteIndirect(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeAccumulator(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeImmediate(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeImplied(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeProgramCounterRelative(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeStack(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeZeroPage(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeZeroPageIndexedIndirect(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeZeroPageIndexedWithX(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeZeroPageIndexedWithY(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeZeroPageIndirectIndexedWithY(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
            bool parseOpcodeIndirect(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode);
        };

    }
}
