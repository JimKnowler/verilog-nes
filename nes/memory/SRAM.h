#pragma once

#include <vector>
#include <cstdint>

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

        /// @brief set a byte in memory to the specified value
        /// @param address byte offset from start of memory
        /// @param value the value to set at specified address
        void set(size_t address, uint8_t value);

        /// @brief retrieve a byte of memory at the specified address
        /// @param address byte offset from start of memory
        uint8_t get(size_t address) const;

        /// @todo set a sequence of opcodes (which will serialise each of them in turn)
        /// @todo handle loops / labels / etc

    private:
        std::vector<uint8_t> memory;
    };
}