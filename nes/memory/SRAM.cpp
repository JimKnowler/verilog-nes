#include "SRAM.h"

namespace memory {
    SRAM::SRAM(size_t size) {
        memory = std::vector<uint8_t>(size);
    }

    SRAM::~SRAM() {

    }

    void SRAM::clear(uint8_t value) {
        for (auto& memoryByte : memory) {
            memoryByte = value;
        }
    }

    void SRAM::write(size_t address, uint8_t value) {
        memory[address] = value;
    }

    uint8_t SRAM::read(size_t address) const {
        return memory[address];
    }
}