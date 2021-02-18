#include <stdlib.h>
#include <verilated.h>
#include <gmock/gmock.h>

int main(int argc, char **argv){
    // initialise Verilator
    Verilated::commandArgs(argc, argv);

    // initialise GoogleMock + GoogleTest
    ::testing::InitGoogleMock(&argc, argv);

    // Run all of the tests
    return RUN_ALL_TESTS();
}