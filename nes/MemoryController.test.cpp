#include <gtest/gtest.h>
using namespace testing;

#include "gtestverilog/gtestverilog.h"
using namespace gtestverilog;

#include "nes/MemoryControllerTestBench.h"
using namespace memorycontrollertestbench;

namespace {
    class MemoryController : public ::testing::Test {
    public:
        void SetUp() override {
        }
        
        void TearDown() override {
        }

        MemoryControllerTestBench testBench;
    };
}

TEST_F(MemoryController, ShouldConstruct) {
}

TEST_F(MemoryController, ShouldReset) {
    testBench.reset();

    // todo: check state
}

// todo: request from each channel individually
// todo: concurrent requests on multiple channels
// todo: concurrent requests on multiple channels
// todo: priority ppu > cpu > debug