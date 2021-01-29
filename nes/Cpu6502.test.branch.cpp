#include "Cpu6502.fixture.h"

TEST_F(Cpu6502, ShouldImplementBCCWhenCarrySet) {
    sram.clear(0);

    Assembler()
        .org(0x8000)
            .NOP()
        .label("start")
            .NOP()
            .NOP()
        .label("my label")
            .LDA().absolute("my data")
            .BCC().relative("my label")
        .org(0x8332)
        .label("my data")
        .word(0x1234)
        .org(0xfffc)
        .word("start")
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