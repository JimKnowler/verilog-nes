cmake_minimum_required(VERSION 3.31.6)

project(verilog-nes
    VERSION 0.2.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

#
# GoogleTest Library
#

# download google test
include(FetchContent)
FetchContent_Declare(
    googletest-verilog
    URL https://github.com/JimKnowler/googletest-verilog/archive/refs/tags/v0.2-rc3.tar.gz
)

FetchContent_MakeAvailable(googletest-verilog)

#
# 6502 
#

file(GLOB_RECURSE CPU6502_ASSEMBLER_SRC_FILES CONFIGURE_DEPENDES "nes/cpu6502/assembler/*.cpp")
file(GLOB_RECURSE CPU6502_TEST_SRC_FILES CONFIGURE_DEPENDES "nes/cpu6502/test/*.cpp")

message(status "CPU6502_ASSEMBLER_SRC_FILES = ${CPU6502_ASSEMBLER_SRC_FILES}")
message(status "CPU6502_TEST_SRC_FILES = ${CPU6502_TEST_SRC_FILES}")

add_executable(cpu6502_test
    ${CPU6502_ASSEMBLER_SRC_FILES}
    ${CPU6502_TEST_SRC_FILES}
)

target_include_directories(cpu6502_test PRIVATE
    ${googletest_verilog_SOURCE_DIR}/include
)

target_include_directories(cpu6502_test PRIVATE
    . 
    ${googletest-verilog_SOURCE_DIR}/include
)

target_link_libraries(cpu6502_test
    gtest
    gmock
    googletest-verilog
)