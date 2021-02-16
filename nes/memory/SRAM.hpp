#pragma once

#include <vector>
#include <cstdint>
#include <ostream>

namespace memory {
    /// @class SRAM
    /// @brief Simple simulation of SRAM
    class SRAM {
    public:
        SRAM(size_t size);
        ~SRAM();

        /// @brief clear the memory to a common value
        /// @param value the value to set all memory as (defaults to 0)
        void clear(uint8_t value = 0);

        /// @brief retrieve the size of memory
        size_t size() const;

        /// @brief set a byte in memory to the specified value
        /// @param address byte offset from start of memory
        /// @param value the value to set at specified address
        void write(size_t address, uint8_t value);

        /// @brief Write a sequence of bytes to memory
        /// @param address byte offset from start of memory to start writing
        /// @param program the assembled byte code of program to write
        void write(size_t address, const std::vector<uint8_t>& program);

        /// @brief retrieve a byte of memory at the specified address
        /// @param address byte offset from start of memory
        uint8_t read(size_t address) const;

    private:
        std::vector<uint8_t> memory;
    };
}

std::ostream& operator<<(std::ostream& os, const memory::SRAM& sram);
