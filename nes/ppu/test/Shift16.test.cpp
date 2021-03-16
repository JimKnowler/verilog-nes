#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/Shift16TestBench.h"
using namespace shift16testbench;

namespace {
    class Shift16 : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);

            testBench.reset();
            testBench.trace.clear();
        }
        
        void TearDown() override {
        }

        Shift16TestBench testBench;
    };
}

TEST_F(Shift16, ShouldConstruct) {

}

TEST_F(Shift16, ShouldInitialiseWithDefaultOutputs) {
    auto& core = testBench.core();

    EXPECT_EQ(core.o_shift_data, 0);
    EXPECT_EQ(core.o_debug_data, 0);
}

TEST_F(Shift16, ShouldShiftOutDefaultValues) {
    auto& core = testBench.core();

    core.i_shift = 1;
    for (int i=0; i<16; i++) {        
        testBench.tick();

        EXPECT_EQ(core.o_shift_data, 0);
    }
}

TEST_F(Shift16, ShouldLoadData) {
    auto& core = testBench.core();

    core.i_load = 1;
    core.i_data = 0b10101010;
    testBench.tick();

    EXPECT_EQ(core.o_debug_data, 0b0000000010101010);
}

TEST_F(Shift16, ShouldShiftOutData) {
    auto& core = testBench.core();

    core.i_load = 1;
    core.i_data = 0b11100110;
    testBench.tick();

    core.i_load = 0;
    core.i_shift = 1;

    std::vector<int> values;
    for (int i=0; i<16; i++) {
        values.push_back(core.o_shift_data);
        testBench.tick();
    }

    EXPECT_THAT(values, ElementsAre(
        0,0,0,0,0,0,0,0,
        1,1,1,0,0,1,1,0
    ));
}

TEST_F(Shift16, ShouldShiftAndLoad) {
    auto& core = testBench.core();

    const uint8_t kInputValues[] = {0b11100110, 0b01010011, 0b11110001, 0};

    std::vector<int> values;
    for (int i=0; i<32; i++) {
        if ((i % 8) == 0) {            
            core.i_data = kInputValues[i/8];
            core.i_load = 1;
        } else {
            core.i_load = 0;
        }

        core.i_shift = 1;
        testBench.tick();

        values.push_back(core.o_shift_data);
    }

    EXPECT_THAT(values, ElementsAre(
        0,0,0,0,0,0,0,0,
        1,1,1,0,0,1,1,0,
        0,1,0,1,0,0,1,1,
        1,1,1,1,0,0,0,1
    ));
}
