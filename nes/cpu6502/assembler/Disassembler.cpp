#include "nes/cpu6502/assembler/Disassembler.hpp"
#include "nes/cpu6502/assembler/Opcodes.hpp"

namespace cpu6502 { namespace assembler {
    namespace {
        const std::map<uint32_t, uint16_t> kAddressingModeByteSizeLookup {
            { kAbsolute, 3 },
            { kAbsolute | kIndexedWithX, 3 },
            { kAbsolute | kIndexedWithY, 3 },
            { kAbsolute | kIndirect, 3 },
            { kAccumulator, 1 },
            { kImmediate, 2 },
            { kImplied, 1 },
            { kRelative, 2 },
            { kZeroPage, 2 },
            { kZeroPage | kIndexedWithX | kIndirect, 2 },
            { kZeroPage | kIndexedWithX, 2 },
            { kZeroPage | kIndexedWithY, 2 },
            { kZeroPage | kIndirect | kIndexedWithY, 2 }
        };
    }

    Disassembler::Disassembler() {
        registerOpcodes();
    }

    Disassembler::DisassembledOpcodes Disassembler::disassemble(const memory::SRAM& sram, uint16_t inPc, int numOpcodes) {
        DisassembledOpcodes disassembledOpcodes;

        uint16_t pc = inPc;     

        for (int i=0; (i < numOpcodes) && (pc < sram.size()); i++) {
            DisassembledOpcode disassembledOpcode;
            disassembledOpcode.pc = pc;
            disassembledOpcode.opcode = sram.read(pc);

            bool success = true;
            
            if (opcodes.find(disassembledOpcode.opcode) == opcodes.end()) {
                char buffer[16];
                sprintf(buffer, "0x%02X", disassembledOpcode.opcode);
                disassembledOpcode.labelOpcode = std::string(buffer) + " ????";
                disassembledOpcode.byteSize = 1;
                disassembledOpcode.addressingMode = kUnknown;
                disassembledOpcode.data.push_back(disassembledOpcode.opcode);

                disassembledOpcodes.push_back(disassembledOpcode);
                success = false;

                break;
            }

            ParseOpcodeInfo parseOpcodeInfo {
                .pc = pc,
                .sram = sram
            };

            auto& opcodeInfo = opcodes.at(disassembledOpcode.opcode);
            disassembledOpcode.labelOpcode = opcodeInfo.label;
            disassembledOpcode.addressingMode = opcodeInfo.addressingMode;
            uint16_t byteSize = kAddressingModeByteSizeLookup.at(opcodeInfo.addressingMode);
            disassembledOpcode.byteSize = byteSize;
            
            if ( (pc + byteSize) <= sram.size() ) {
                for (uint16_t i = 0; i<byteSize; i++) {
                    disassembledOpcode.data.push_back(sram.read(pc + i));
                }

                switch (opcodeInfo.addressingMode) {
                    case kAbsolute:
                        success &= parseOpcodeAbsolute(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kAbsolute | kIndirect | kIndexedWithX:
                        success &= parseOpcodeAbsoluteIndexedIndirect(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kAbsolute | kIndexedWithX:
                        success &= parseOpcodeAbsoluteIndexedWithX(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kAbsolute | kIndexedWithY:
                        success &= parseOpcodeAbsoluteIndexedWithY(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kAbsolute | kIndirect:
                        success &= parseOpcodeAbsoluteIndirect(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kAccumulator:
                        success &= parseOpcodeAccumulator(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kImmediate:
                        success &= parseOpcodeImmediate(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kImplied:
                        // also for kStack
                        success &= parseOpcodeImplied(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kRelative:
                        success &= parseOpcodeProgramCounterRelative(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kZeroPage:
                        success &= parseOpcodeZeroPage(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kZeroPage | kIndexedWithX | kIndirect:
                        success &= parseOpcodeZeroPageIndexedIndirect(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kZeroPage | kIndexedWithX:
                        success &= parseOpcodeZeroPageIndexedWithX(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kZeroPage | kIndexedWithY:
                        success &= parseOpcodeZeroPageIndexedWithY(parseOpcodeInfo, disassembledOpcode);
                        break;
                    case kZeroPage | kIndirect | kIndexedWithY:
                        success &= parseOpcodeZeroPageIndirectIndexedWithY(parseOpcodeInfo, disassembledOpcode);
                        break;
                    default:
                        printf("unsupported addressing mode [%u]\n", unsigned(opcodeInfo.addressingMode));
                        assert(false);
                }
            }

            if (!success) {
                break;
            }

            disassembledOpcodes.push_back(disassembledOpcode);
            pc += byteSize;
        }
        
        return disassembledOpcodes;
    }

    void Disassembler::registerOpcodes() {
        #define OPCODE(_x) { _x op; registerOpcode(#_x, &op); }
        # include "OpcodeList.inl"
        #undef OPCODE
    }

    void Disassembler::registerOpcode(const std::string& label, const Opcode* opcode) {
        const Opcode::AddressingModes& addressingModes = opcode->addressingModes();

        for (auto it = addressingModes.begin(); it != addressingModes.end(); it ++ ) {
            uint32_t addressingMode = it->first;
            uint8_t opcode = it->second;

            OpcodeInfo opcodeInfo {
                .label = label,
                .addressingMode = addressingMode
            };

            opcodes[opcode] = opcodeInfo;
        }
    }

    bool Disassembler::parseOpcodeAbsolute(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint16_t address = (info.sram.read(info.pc + 2) << 8) | info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "$0x%04X", address);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeAbsoluteIndexedIndirect(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint16_t address = (info.sram.read(info.pc + 2) << 8) | info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "($0x%04X,x)", address);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeAbsoluteIndexedWithX(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint16_t address = (info.sram.read(info.pc + 2) << 8) | info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "$0x%04X,x", address);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeAbsoluteIndexedWithY(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint16_t address = (info.sram.read(info.pc + 2) << 8) | info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "$0x%4X,y", address);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeAbsoluteIndirect(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint16_t address = (info.sram.read(info.pc + 2) << 8) | info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "($0x%04X)", address);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeAccumulator(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        outOpcode.labelOperands = "A";

        return true;
    }

    bool Disassembler::parseOpcodeImmediate(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint8_t data = info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "#$0x%02X", data);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeImplied(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        return true;
    }

    bool Disassembler::parseOpcodeProgramCounterRelative(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint8_t data = info.sram.read(info.pc + 1);

        // combine relative-offset with pc to create an address
        uint16_t address = info.pc + 2 + *(reinterpret_cast<int8_t*>(&data));
        
        char buffer[16];
        sprintf(buffer, "$0x%04X", address);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeStack(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        return true;
    }

    bool Disassembler::parseOpcodeZeroPage(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint8_t address = info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "$0x%02X", address);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeZeroPageIndexedIndirect(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint8_t address = info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "($0x%02X,x)", address);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeZeroPageIndexedWithX(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint8_t address = info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "$0x%02X,x", address);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeZeroPageIndexedWithY(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint8_t address = info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "$0x%02X,y", address);
        outOpcode.labelOperands = buffer;

        return true;
    }

    bool Disassembler::parseOpcodeZeroPageIndirectIndexedWithY(const ParseOpcodeInfo& info, DisassembledOpcode& outOpcode) {
        uint8_t address = info.sram.read(info.pc + 1);
        
        char buffer[16];
        sprintf(buffer, "($0x%02X),y", address);
        outOpcode.labelOperands = buffer;

        return true;
    }
} // assembler
} // cpu6502
