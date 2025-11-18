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
    URL https://github.com/JimKnowler/googletest-verilog/archive/refs/tags/v0.2-rc7.tar.gz
)

FetchContent_MakeAvailable(googletest-verilog)

#
# 6502 Test
#

file(GLOB_RECURSE CPU6502_ASSEMBLER_SRC_FILES CONFIGURE_DEPENDS "nes/cpu6502/assembler/*.cpp")
file(GLOB_RECURSE CPU6502_TEST_SRC_FILES CONFIGURE_DEPENDS "nes/cpu6502/test/*.cpp")
file(GLOB_RECURSE NES_MEMORY_SRC_FILES CONFIGURE_DEPENDS "nes/memory/*.cpp")

message(status "CPU6502_ASSEMBLER_SRC_FILES = ${CPU6502_ASSEMBLER_SRC_FILES}")
message(status "CPU6502_TEST_SRC_FILES = ${CPU6502_TEST_SRC_FILES}")
message(status "NES_MEMORY_SRC_FILES = ${NES_MEMORY_SRC_FILES}")

add_executable(cpu6502_test
    ${CPU6502_TEST_SRC_FILES}
    ${CPU6502_ASSEMBLER_SRC_FILES}
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
# NES Test
#

file(GLOB_RECURSE NES_TEST_SRC_FILES CONFIGURE_DEPENDS "nes/nes/test/*.cpp")

message(status "CPU6502_ASSEMBLER_SRC_FILES = ${CPU6502_ASSEMBLER_SRC_FILES}")
message(status "NES_TEST_SRC_FILES = ${NES_TEST_SRC_FILES}")

add_executable(nes_test
    ${NES_TEST_SRC_FILES}
    ${CPU6502_ASSEMBLER_SRC_FILES}
    ${NES_MEMORY_SRC_FILES}
)

target_include_directories(nes_test PRIVATE
    .
)

target_link_libraries(nes_test
    gtest
    gmock
    gtestverilog
)

googletest_verilog_testbench(nes_test SOURCE nes/nes/ClockEnable.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu)
googletest_verilog_testbench(nes_test SOURCE nes/nes/NES.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu)
googletest_verilog_testbench(nes_test SOURCE nes/nes/VideoOutput.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu)
googletest_verilog_testbench(nes_test SOURCE nes/nes/CPUMemoryMap.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu)
googletest_verilog_testbench(nes_test SOURCE nes/nes/PPUMemoryMap.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu)
googletest_verilog_testbench(nes_test SOURCE nes/nes/Cpu2A03.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu)

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


#
# PPU Test
#

file(GLOB_RECURSE PPU_TEST_SRC_FILES CONFIGURE_DEPENDS "nes/ppu/test/*.cpp")

message(status "PPU_TEST_SRC_FILES = ${PPU_TEST_SRC_FILES}")

add_executable(ppu_test
    ${PPU_TEST_SRC_FILES}
    ${CPU6502_ASSEMBLER_SRC_FILES}
    ${NES_MEMORY_SRC_FILES}
)

target_include_directories(ppu_test PRIVATE
    .
)

target_link_libraries(ppu_test
    gtest
    gmock
    gtestverilog
)

googletest_verilog_testbench(ppu_test SOURCE nes/ppu/vga/VGAExample.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/vga/VGAGenerator.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/vga/VGAOutput.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/PPUAttributeAddress.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/PPU.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/PaletteLookupRGB.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/PPUIncrementX.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/PPUPatternTableAddress.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/PPUChipEnable.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/Shift8.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/Shift16.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/PPUTileAddress.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)
googletest_verilog_testbench(ppu_test SOURCE nes/ppu/PPUIncrementY.v INCLUDE_DIRS nes/nes nes/cpu6502 nes/ppu nes/ppu/vga)

# VGAGenerator3x2
#verilator_cc_library(
#    name = "VGAGenerator3x2",
#    mtop = "VGAGenerator",
#    srcs = ["ppu/vga/VGAGenerator.v"],
#    vopts = [
#        "-Wall",
#        "-GWIDTH=11'd6",                # note: specifying bit-width of constants
#        "-GHEIGHT=11'd3",
#        "-GWIDTH_VISIBLE=11'd3",
#        "-GHEIGHT_VISIBLE=11'd2"
#    ]
#)


# VGAOutput3x2
# verilator_cc_library(
#    name = "VGAOutput3x2",
#    mtop = "VGAOutput",
#    srcs = ["ppu/vga/VGAOutput.v"],
#    vopts = [
#        "-Wall",
#        "-GWIDTH=11'd7",                # note: specifying bit-width of constants
#        "-GHEIGHT=11'd5",
#        "-GWIDTH_VISIBLE=11'd3",
#        "-GHEIGHT_VISIBLE=11'd2",
#        "-GFRONT_PORCH_X=11'd1",
#        "-GBACK_PORCH_X=11'd1",
#        "-GFRONT_PORCH_Y=11'd1",
#        "-GBACK_PORCH_Y=11'd1",
#    ]
#)


#
# CPU Emulator
#

add_executable(emulator_cpu
    nes/emulator/EmulatorCPU.cpp
    nes/emulator/RendererCPU.cpp
    ${NES_MEMORY_SRC_FILES}
    ${CPU6502_ASSEMBLER_SRC_FILES}
)

target_include_directories(emulator_cpu PRIVATE
    .
)

target_link_libraries(emulator_cpu
    gtestverilog
    "-framework OpenGL"
    "-framework GLUT"
)

googletest_verilog_testbench(emulator_cpu SOURCE nes/cpu6502/Cpu6502.v INCLUDE_DIRS nes/cpu6502 nes/nes nes/ppu)
