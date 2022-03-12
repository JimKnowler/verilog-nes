#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/Shift8TestBench.h"
using namespace shift8testbench;

namespace {
    class Shift8 : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);

            testBench.reset();
            testBench.trace.clear();

            testBench.core().i_ce = 1;
        }
        
        void TearDown() override {
        }

        void helperLoadValue(uint8_t value) {
            auto& core = testBench.core();
            core.i_shift = 1;
            core.i_load = 1;

            for (int i=0; i<8; i++) {
                core.i_data = (value >> i) & 1;
                testBench.tick();
            }
        }

        Shift8TestBench testBench;
    };
}

TEST_F(Shift8, ShouldConstruct) {
    auto& core = testBench.core();
    ASSERT_EQ(0, core.o_debug_data);
}

TEST_F(Shift8, ShouldShiftWithLoad) {
    auto& core = testBench.core();
    core.i_shift = 1;
    core.i_load = 1;
    core.i_data = 1;
    testBench.tick();

    ASSERT_EQ(1 << 7, core.o_debug_data);
}

TEST_F(Shift8, ShouldShiftWithoutLoad) {
    auto& core = testBench.core();
    core.i_shift = 1;
    core.i_load = 0;
    core.i_data = 1;
    testBench.tick();

    ASSERT_EQ(0, core.o_debug_data);
}

TEST_F(Shift8, ShouldShiftRepeatedly) {
    auto& core = testBench.core();

    const uint8_t kTestValue = 0x3a;
    helperLoadValue(kTestValue);
    
    ASSERT_EQ(kTestValue, core.o_debug_data);
}

TEST_F(Shift8, ShouldLoadWithoutShift) {
    auto& core = testBench.core();

    const uint8_t kTestValue = 0b01100110;
    helperLoadValue(kTestValue);

    core.i_load = 1;
    core.i_data = 0;
    core.i_shift = 0;
    testBench.tick();

    ASSERT_EQ(kTestValue & ~0x80, core.o_debug_data);
}

TEST_F(Shift8, ShouldReadOffsetBit) {
    auto& core = testBench.core();

    const uint8_t kTestValue = 0b10010010;
    helperLoadValue(kTestValue);

    for (int i=0; i<8; i++) {
        core.i_offset = i;
        core.eval();
        EXPECT_EQ((kTestValue >> i) & 0x01, core.o_shift_data);        
    }
}

// todo: posedge or negedge?
