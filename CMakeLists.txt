cmake_minimum_required(VERSION 3.31.6)

project(verilog-nes
    VERSION 0.2.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

#
# googletest-verilog
#

include(FetchContent)
FetchContent_Declare(
    googletest-verilog
    URL https://github.com/JimKnowler/googletest-verilog/archive/refs/tags/v0.2-rc6.tar.gz
)

FetchContent_MakeAvailable(googletest-verilog)

#
# 6502 
#

file(GLOB_RECURSE CPU6502_ASSEMBLER_SRC_FILES CONFIGURE_DEPENDES "nes/cpu6502/assembler/*.cpp")
file(GLOB_RECURSE CPU6502_TEST_SRC_FILES CONFIGURE_DEPENDES "nes/cpu6502/test/*.cpp")
file(GLOB_RECURSE NES_MEMORY_SRC_FILES CONFIGURE_DEPENDES "nes/memory/*.cpp")

message(status "CPU6502_ASSEMBLER_SRC_FILES = ${CPU6502_ASSEMBLER_SRC_FILES}")
message(status "CPU6502_TEST_SRC_FILES = ${CPU6502_TEST_SRC_FILES}")
message(status "NES_MEMORY_SRC_FILES = ${NES_MEMORY_SRC_FILES}")

add_executable(cpu6502_test
    ${CPU6502_ASSEMBLER_SRC_FILES}
    ${CPU6502_TEST_SRC_FILES}
    ${NES_MEMORY_SRC_FILES}
)

target_include_directories(cpu6502_test PRIVATE
    .
)

target_link_libraries(cpu6502_test
    gtest
    gmock
    gtestverilog
)

googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/Cpu6502.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/AddressBusRegister.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/ALU.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/DL.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/DOR.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/IR.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/PCH.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/PCL.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/ProcessorStatus.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/Register.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/Routing.v INCLUDE_DIRS nes/cpu6502)
googletest_verilog_testbench(cpu6502_test SOURCE nes/cpu6502/TCU.v INCLUDE_DIRS nes/cpu6502)

#
# NES Emulator
#

add_executable(nes_emulator
    nes/emulator/EmulatorNES.cpp
    ${NES_MEMORY_SRC_FILES}
)

target_include_directories(nes_emulator PRIVATE
    .
)

target_link_libraries(nes_emulator
    gtestverilog
    "-framework OpenGL"
    "-framework GLUT"
)

googletest_verilog_testbench(nes_emulator SOURCE nes/nes/NES.v INCLUDE_DIRS nes/cpu6502 nes/nes nes/ppu)
