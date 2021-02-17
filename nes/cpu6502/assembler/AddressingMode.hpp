#pragma once

namespace cpu6502 {
    namespace assembler {
        enum AddressingMode : uint32_t {
            kImplied = 0,
            kStack = kImplied,                      // note: stack has same byte size and
                                                    //       same opcode appearance as implied
            kImmediate = 1 << 0,
            kAbsolute = 1 << 1,
            kAccumulator = 1 << 2,
            kRelative = 1 << 3,
            kIndirect = 1 << 4,
            kIndexedWithX = 1 << 5,
            kIndexedWithY = 1 << 6,
            kZeroPage = 1 << 7,

            kUnknown = 0xffff
        };
    }
}
