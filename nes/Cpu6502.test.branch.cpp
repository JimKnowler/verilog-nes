#include "Cpu6502.fixture.h"

TEST_F(Cpu6502, ShouldImplementBCCWhenCarrySet) {
    sram.clear(0);

    Assembler()
            .NOP()
            .NOP()
            .NOP()
        .label("my label")
            .LDA().absolute(0x22)
            .BCC().relative("my label")
        .compileTo(sram);

    std::cout << sram;

    EXPECT_FALSE("implement this");
}

TEST_F(Cpu6502, ShouldImplementBCCWhenCarryClear) {
    EXPECT_FALSE("implement this");
}

TEST_F(Cpu6502, ShouldImplementBCCWhenCarryClearAndNewPage) {
    EXPECT_FALSE("implement this");
}