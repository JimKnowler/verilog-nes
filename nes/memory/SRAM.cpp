#include "SRAM.h"

namespace memory {
    SRAM::SRAM(size_t size) {
        memory = std::vector<uint8_t>(size);
    }

    SRAM::~SRAM() {

    }

    size_t SRAM::size() const {
        return memory.size();
    }

    void SRAM::clear(uint8_t value) {
        for (auto& memoryByte : memory) {
            memoryByte = value;
        }
    }

    void SRAM::write(size_t address, uint8_t value) {
        memory[address] = value;
    }

    void SRAM::write(size_t address, const std::vector<uint8_t>& program) {
        for (size_t i=0; i<program.size(); i++) {
            write(i, program[i]);
        }
    }

    uint8_t SRAM::read(size_t address) const {
        return memory[address];
    }
}

std::ostream& operator<<(std::ostream& os, const memory::SRAM& sram) {
    const size_t kRowSize = 16;
    const size_t kNumRows = sram.size() / kRowSize;

    std::vector<uint8_t> rowCache;
    bool hasReportedRun = false;

    size_t readIndex = 0;

    for (size_t rowIndex = 0; rowIndex < kNumRows; rowIndex ++) {
        std::vector<uint8_t> row;
        for (size_t i=0; i < kRowSize; i++, readIndex++) {
            row.push_back(sram.read(readIndex));
        }

        if (row == rowCache) {
            if (!hasReportedRun) {
                printf("*\n");
                hasReportedRun = true;
            }
        } else {
            printf("%08zx  ", rowIndex * kRowSize);

            const size_t kChunkSize = 8;
            const size_t kNumChunks = kRowSize / kChunkSize;
            
            for (size_t chunk=0; chunk < kNumChunks; chunk++) {
                size_t chunkStart = chunk * kChunkSize;
                size_t chunkEnd = chunkStart + kChunkSize;

                for (uint8_t i=chunkStart; i<chunkEnd; i++) {
                    printf("%02x ", row[i]);
                }

                printf(" ");
            }

            printf("|");
            for (size_t i=0; i<kRowSize; i++) {
                char c = char(row[i]);

                printf("%1c", isprint(c) ? c : '.');
            }

            printf("|\n");

            rowCache = row;
        }
    }

    if (hasReportedRun) {
        printf("%08zx\n", sram.size());
    }

    return os;
}
