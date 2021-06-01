#include "SRAM.hpp"

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
        assert(address < memory.size());

        memory[address] = value;
    }

    void SRAM::write(size_t address, const std::vector<uint8_t>& program) {
        assert( (address + program.size()) <= memory.size());

        for (size_t i=0; i<program.size(); i++) {
            write(i + address, program[i]);
        }
    }

    uint8_t SRAM::read(size_t address) const {
        assert(address < memory.size());
        
        return memory[address];
    }
}

std::ostream& operator<<(std::ostream& os, const memory::SRAM& sram) {
    // ostream formatting based on 'hexdump' tool 

    const size_t kRowSize = 16;
    const size_t kNumRows = sram.size() / kRowSize;
    char buffer[16];

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
            hasReportedRun = false;

            sprintf(buffer, "%08zx  ", rowIndex * kRowSize);
            os << buffer;

            const size_t kChunkSize = 8;
            const size_t kNumChunks = kRowSize / kChunkSize;
            
            for (size_t chunk=0; chunk < kNumChunks; chunk++) {
                size_t chunkStart = chunk * kChunkSize;
                size_t chunkEnd = chunkStart + kChunkSize;

                for (uint8_t i=chunkStart; i<chunkEnd; i++) {
                    sprintf(buffer, "%02x ", row[i]);
                    os << buffer;
                }

                os << " ";
            }

            os << "|";
            for (size_t i=0; i<kRowSize; i++) {
                char c = char(row[i]);

                sprintf(buffer, "%1c", isprint(c) ? c : '.');
                os << buffer;
            }

            os << "|\n";

            rowCache = row;
        }
    }

    if (hasReportedRun) {
        sprintf(buffer, "%08zx\n", sram.size());
        os << buffer;
    }

    return os;
}
