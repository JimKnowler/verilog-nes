#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/ALUTestBench.h"
using namespace alutestbench;

namespace {
    class ALU : public ::testing::Test {
    public:
        void SetUp() override {
            testBench.setClockPolarity(1);
            testBench.core().i_ce = 1;
        }
        
        void TearDown() override {
        }

        ALUTestBench testBench;
    };
}

TEST_F(ALU, ShouldConstruct) {
}

TEST_F(ALU, ShouldOutputDefaultValues) {
    auto& core = testBench.core();

    EXPECT_EQ(0, core.o_avr);
    EXPECT_EQ(0, core.o_acr);
}

TEST_F(ALU, ShouldNotPassThroughDuringPhi1) {
    auto& core = testBench.core();

    core.i_clk = 0;
    core.i_db = 0xDB;
    core.i_db_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;
    core.eval();

    EXPECT_EQ(0, core.o_add);
}

TEST_F(ALU, ShouldNotPassThroughDuringPhi2) {
    auto& core = testBench.core();

    core.i_clk = 0;
    core.i_db = 0xDB;
    core.i_db_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;
    core.eval();

    // latch in at rising edge of phi2
    core.i_clk = 1;
    core.eval();

    // change input during phi2
    core.i_db = 0xAF;
    core.eval();
    EXPECT_EQ(0xDB, core.o_add);
}

TEST_F(ALU, ShouldLatchAtRisingEdgeOfPhi2) {
    auto& core = testBench.core();

    core.i_clk = 0;    
    core.i_db = 0xAE;
    core.i_db_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;
    core.eval();

    // rising edge of phi2
    core.i_clk = 1;
    core.eval();
    EXPECT_EQ(0xAE, core.o_add);
}

TEST_F(ALU, ShouldAddDbToZero) {
    auto& core = testBench.core();

    core.i_db = 0xAE;
    core.i_db_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;
    
    testBench.tick();
    EXPECT_EQ(0xAE, core.o_add);
}

TEST_F(ALU, ShouldInvertDb) {
    auto& core = testBench.core();

    core.i_db = 0xAE;
    core.i_db_n_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;

    testBench.tick();
    EXPECT_EQ(0x51, core.o_add);
}

TEST_F(ALU, ShouldReset) {
    auto& core = testBench.core();

    // clock in data
    core.i_db = 0xAE;
    core.i_db_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;
    testBench.tick();

    // reset (during phi1)
    core.i_clk = 0;
    core.i_reset_n = 0;
    core.eval();
    core.i_reset_n = 1;
    core.eval();
    EXPECT_EQ(0, core.o_add);
}

TEST_F(ALU, ShouldAddAdlToZero) {
    auto& core = testBench.core();

    core.i_adl = 0xBC;
    core.i_adl_add = 1;
    core.i_0_add = 1;
    core.i_sums = 1;

    testBench.tick();
    EXPECT_EQ(0xBC, core.o_add);
}

TEST_F(ALU, ShouldAddAdlToSb) {
    auto& core = testBench.core();

    core.i_adl = 0x11;
    core.i_adl_add = 1;
    core.i_sb = 0x22;
    core.i_sb_add = 1;
    core.i_sums = 1;
    
    testBench.tick();
    EXPECT_EQ(0x33, core.o_add);
}

TEST_F(ALU, ShouldAndAdlWithSb) {
    auto& core = testBench.core();

    core.i_adl = 0x9F;
    core.i_adl_add = 1;
    core.i_sb = 0x3F;
    core.i_sb_add = 1;
    core.i_ands = 1;

    testBench.tick();
    EXPECT_EQ(0x1F, core.o_add);
}

TEST_F(ALU, ShouldEorAdlWithSb) {
    auto& core = testBench.core();

    core.i_adl = 0x9F;
    core.i_adl_add = 1;
    core.i_sb = 0x3F;
    core.i_sb_add = 1;
    core.i_eors = 1;
    
    testBench.tick();
    EXPECT_EQ(0xA0, core.o_add);
}

TEST_F(ALU, ShouldOrAdlWithSb) {
    auto& core = testBench.core();

    core.i_adl = 0x9F;
    core.i_adl_add = 1;
    core.i_sb = 0x3F;
    core.i_sb_add = 1;
    core.i_ors = 1;
    
    testBench.tick();
    EXPECT_EQ(0xBF, core.o_add);
}

TEST_F(ALU, ShouldShiftRightSb) {
    auto& core = testBench.core();

    core.i_sb = 0xF0;
    core.i_sb_add = 1;
    core.i_srs = 1;
    
    testBench.tick();
    EXPECT_EQ(0x78, core.o_add);
}

TEST_F(ALU, ShouldShiftRightWithCarryIn) {
    auto& core = testBench.core();

    core.i_sb = 0xF0;
    core.i_sb_add = 1;
    core.i_1_addc = 1;
    core.i_srs = 1;
    
    testBench.tick();
    EXPECT_EQ(0xF8, core.o_add);
}

TEST_F(ALU, ShouldAddWithCarryIn) {
    auto& core = testBench.core();
    
    core.i_adl = 0x11;
    core.i_adl_add = 1;
    core.i_sb = 0x32;
    core.i_sb_add = 1;
    core.i_sums = 1;
    core.i_1_addc = 1;
    
    testBench.tick();
    EXPECT_EQ(0x44, core.o_add);
}

TEST_F(ALU, ShouldOutputACRDuringShiftRight) {
    auto& core = testBench.core();
    
    const std::map<uint8_t, uint8_t> testCases = {
        {0, 0},
        {2, 0},
        {1, 1},
        {3, 1},
        {0xff, 1}
    };

    for (auto& testCase: testCases) {
        const uint8_t kTestValue = testCase.first;
        const uint8_t kExpectedAcr = testCase.second;

        core.i_sb = kTestValue;
        core.i_sb_add = 1;
        core.i_srs = 1;
        
        testBench.tick();
        EXPECT_EQ(kExpectedAcr, core.o_acr);
    }
}

TEST_F(ALU, ShouldOutputACRDuringSum) {
    auto& core = testBench.core();
    
    // examples from: http://www.6502.org/tutorials/vflag.html
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> testCases = {
        {{0x00, 0x00}, 0},
        {{0x01, 0x01}, 0},      //    1 +  1 = 2
        {{0x01, 0xFF}, 1},      //    1 + -1 = 0
        {{0x7F, 0x01}, 0},      //  127 +  1 = 128
        {{0x80, 0xFF}, 1},      // -128 + -1 = -129
    };

    for (auto& testCase: testCases) {
        const uint8_t kTestValue1 = testCase.first.first;
        const uint8_t kTestValue2 = testCase.first.second;
        const uint8_t kExpectedAcr = testCase.second;

        core.i_sb = kTestValue1;
        core.i_sb_add = 1;
        core.i_db = kTestValue2;
        core.i_db_add = 1;
        core.i_sums = 1;
        
        testBench.tick();
        EXPECT_EQ(kExpectedAcr, core.o_acr);
    }
}

TEST_F(ALU, ShouldOutputAVRDuringSum) {
    auto& core = testBench.core();
    
    // examples from: http://www.6502.org/tutorials/vflag.html
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> testCases = {
        {{0x00, 0x00}, 0},
        {{0x01, 0x01}, 0},      //    1 +  1 = 2
        {{0x01, 0xFF}, 0},      //    1 + -1 = 0
        {{0x7F, 0x01}, 1},      //  127 +  1 = 128
        {{0x80, 0xFF}, 1},      // -128 + -1 = -129
        {{0x00, 0xFF}, 0},      //    0 + -1 = -1
    };

    for (auto& testCase: testCases) {
        const uint8_t kTestValue1 = testCase.first.first;
        const uint8_t kTestValue2 = testCase.first.second;
        const uint8_t kExpectedAvr = testCase.second;

        core.i_sb = kTestValue1;
        core.i_sb_add = 1;
        core.i_db = kTestValue2;
        core.i_db_add = 1;
        core.i_sums = 1;
        
        testBench.tick();
        EXPECT_EQ(kExpectedAvr, core.o_avr);
    }
}

TEST_F(ALU, ShouldOutputACRDuringSumWithCarryIn) {
    auto& core = testBench.core();
    
    // examples from: http://www.6502.org/tutorials/vflag.html
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> testCases = {
        {{0x00, 0x00}, 0},      //    0 +  0  + 1 = 1
        {{0x01, 0x01}, 0},      //    1 +  1  + 1 = 3
        {{0x01, 0xFF}, 1},      //    1 + 255 + 1 = 257
        {{0x7F, 0x01}, 0},      //  127 +  1  + 1 = 129
        {{0x80, 0xFF}, 1},      //  128 + 255 + 1 = 384
    };

    for (auto& testCase: testCases) {
        const uint8_t kTestValue1 = testCase.first.first;
        const uint8_t kTestValue2 = testCase.first.second;
        const uint8_t kExpectedAcr = testCase.second;

        core.i_sb = kTestValue1;
        core.i_sb_add = 1;
        core.i_db = kTestValue2;
        core.i_db_add = 1;
        core.i_sums = 1;
        core.i_1_addc = 1;
        
        testBench.tick();
        EXPECT_EQ(kExpectedAcr, core.o_acr);
    }
}

TEST_F(ALU, ShouldOutputAVRDuringSumWithCarryIn) {
    auto& core = testBench.core();
    
    // examples from: http://www.6502.org/tutorials/vflag.html
    const std::map<std::pair<uint8_t, uint8_t>, uint8_t> testCases = {
        {{0x00, 0x00}, 0},      //    0 +  0   + 1 = 1
        {{0x01, 0x01}, 0},      //    1 +  1   + 1 = 3
        {{0x01, 0xFF}, 0},      //    1 + -1   + 1 = 1
        {{0x7F, 0x01}, 1},      //  127 +  1   + 1 = 129
        {{0x80, 0xFF}, 0},      // -128 + -1   + 1 = -128
        {{0x00, 0xFF}, 0},      //    0 + -1   + 1 = -1
        {{0xFF, 0x7F}, 0},      //   -1 +  127 + 1 = 127
    };

    for (auto& testCase: testCases) {
        const uint8_t kTestValue1 = testCase.first.first;
        const uint8_t kTestValue2 = testCase.first.second;
        const uint8_t kExpectedAvr = testCase.second;

        core.i_sb = kTestValue1;
        core.i_sb_add = 1;
        core.i_db = kTestValue2;
        core.i_db_add = 1;
        core.i_sums = 1;
        core.i_1_addc = 1;
        
        testBench.tick();
        EXPECT_EQ(kExpectedAvr, core.o_avr);
    }
}

TEST_F(ALU, ShouldLatchAVRAtRisingEdgeOfPhi2) {
    auto& core = testBench.core();
    
    const uint8_t kTestValue1 = 0x7F;
    const uint8_t kTestValue2 = 0x01;
        
    core.i_sb = kTestValue1;
    core.i_sb_add = 1;
    core.i_db = kTestValue2;
    core.i_db_add = 1;
    core.i_sums = 1;
    
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(0, core.o_avr);
    
    core.i_clk = 1;
    core.eval();
    EXPECT_EQ(1, core.o_avr);
}

TEST_F(ALU, ShouldLatchACRAtRisingEdgeOfPhi2) {
    auto& core = testBench.core();

    const uint8_t kTestValue1 = 0x80;
    const uint8_t kTestValue2 = 0xFF;

    core.i_sb = kTestValue1;
    core.i_sb_add = 1;
    core.i_db = kTestValue2;
    core.i_db_add = 1;
    core.i_sums = 1;
    core.i_clk = 0;
    core.eval();
    EXPECT_EQ(0, core.o_acr);

    core.i_clk = 1;
    core.eval();
    EXPECT_EQ(1, core.o_acr);
}

TEST_F(ALU, ShouldNotLatchACRWhenClockDisabled) {
    auto& core = testBench.core();
    core.i_ce = 0;

    const uint8_t kTestValue1 = 0x80;
    const uint8_t kTestValue2 = 0xFF;

    core.i_sb = kTestValue1;
    core.i_sb_add = 1;
    core.i_db = kTestValue2;
    core.i_db_add = 1;
    core.i_sums = 1;
    
    testBench.tick(2);

    EXPECT_EQ(0, core.o_acr);
}

TEST_F(ALU, ShouldNotLatchAVRWhenClockDisabled) {
    auto& core = testBench.core();
    core.i_ce = 0;
    
    const uint8_t kTestValue1 = 0x7F;
    const uint8_t kTestValue2 = 0x01;
        
    core.i_sb = kTestValue1;
    core.i_sb_add = 1;
    core.i_db = kTestValue2;
    core.i_db_add = 1;
    core.i_sums = 1;
    
    testBench.tick(2);
    EXPECT_EQ(0, core.o_avr);
}

TEST_F(ALU, ShouldNotModifyAddWhenClockDisabled) {
    auto& core = testBench.core();
    core.i_ce = 0;
    
    core.i_adl = 0x11;
    core.i_adl_add = 1;
    core.i_sb = 0x32;
    core.i_sb_add = 1;
    core.i_sums = 1;
    core.i_1_addc = 1;
    
    testBench.tick();
    EXPECT_EQ(0, core.o_add);
}
