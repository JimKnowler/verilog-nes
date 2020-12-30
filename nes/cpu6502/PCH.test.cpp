#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/PCHTestBench.h"
using namespace pchtestbench;

namespace {
    class PCH : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        PCHTestBench testBench;
    };
}

TEST_F(PCH, ShouldConstruct) {
}

TEST_F(PCH, ShouldReset) {
    testBench.reset();
}