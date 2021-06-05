load("@rules_verilator//verilator:defs.bzl", "verilator_cc_library")
load("@gtestverilog//gtestverilog:defs.bzl", "gtest_verilog_testbench")

verilator_cc_library(
    name = "Cpu6502",
    srcs = [
        "cpu6502/Cpu6502.v", 
        "cpu6502/Decoder.v",
        "cpu6502/PCL.v",
        "cpu6502/PCH.v",
        "cpu6502/DL.v",
        "cpu6502/Register.v",
        "cpu6502/AddressBusRegister.v",
        "cpu6502/DOR.v",
        "cpu6502/Routing.v",
        "cpu6502/TCU.v",
        "cpu6502/IR.v",
        "cpu6502/ALU.v",
        "cpu6502/ALUFullAdder.v",
        "cpu6502/ProcessorStatus.v"
    ],
)

gtest_verilog_testbench(
    name = "Cpu6502TestBench",
    deps = [":Cpu6502"]
)

verilator_cc_library(
    name = "PCH",
    srcs = ["cpu6502/PCH.v"]
)

gtest_verilog_testbench(
    name = "PCHTestBench",
    deps = [":PCH"]
)

verilator_cc_library(
    name = "PCL",
    srcs = ["cpu6502/PCL.v"]
)

gtest_verilog_testbench(
    name = "PCLTestBench",
    deps = [":PCL"]
)

verilator_cc_library(
    name = "AddressBusRegister",
    srcs = ["cpu6502/AddressBusRegister.v"]
)

gtest_verilog_testbench(
    name = "AddressBusRegisterTestBench",
    deps = [":AddressBusRegister"]
)

verilator_cc_library(
    name = "DL",
    srcs = ["cpu6502/DL.v"]
)

gtest_verilog_testbench(
    name = "DLTestBench",
    deps = [":DL"]
)

verilator_cc_library(
    name = "Routing",
    srcs = ["cpu6502/Routing.v"]
)

gtest_verilog_testbench(
    name = "RoutingTestBench",
    deps = [":Routing"]
)

verilator_cc_library(
    name = "Register",
    srcs = ["cpu6502/Register.v"]
)

gtest_verilog_testbench(
    name = "RegisterTestBench",
    deps = [":Register"]
)

verilator_cc_library(
    name = "DOR",
    srcs = ["cpu6502/DOR.v"]
)

gtest_verilog_testbench(
    name = "DORTestBench",
    deps = [":DOR"]
)

verilator_cc_library(
    name = "TCU",
    srcs = ["cpu6502/TCU.v"]
)

gtest_verilog_testbench(
    name = "TCUTestBench",
    deps = [":TCU"]
)

verilator_cc_library(
    name = "IR",
    srcs = ["cpu6502/IR.v"]
)

gtest_verilog_testbench(
    name = "IRTestBench",
    deps = [":IR"]
)

verilator_cc_library(
    name = "ALU",
    srcs = ["cpu6502/ALU.v", "cpu6502/ALUFullAdder.v"],
)

gtest_verilog_testbench(
    name = "ALUTestBench",
    deps = [":ALU"]
)

verilator_cc_library(
    name = "ProcessorStatus",
    srcs = ["cpu6502/ProcessorStatus.v"],
)

gtest_verilog_testbench(
    name = "ProcessorStatusTestBench",
    deps = [":ProcessorStatus"]
)

cc_test(
    name = "test-cpu6502",
    srcs = glob(
        include =[
            "**/*.cpp",
            "**/*.h",
            "**/*.hpp",
            "**/*.inl"
        ],
        exclude = [
            "emulator/**/*",
            "ppu/**/*",
            "nes/**/*"
        ]
    ) + [
        ":Cpu6502TestBench",
        ":PCHTestBench",
        ":PCLTestBench",
        ":AddressBusRegisterTestBench",
        ":DLTestBench",
        ":RoutingTestBench",
        ":RegisterTestBench",
        ":DORTestBench",
        ":TCUTestBench",
        ":IRTestBench",
        ":ALUTestBench",
        ":ProcessorStatusTestBench"
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":Cpu6502",
        ":PCH",
        ":PCL",
        ":AddressBusRegister",
        ":DL",
        ":Routing",
        ":Register",
        ":DOR",
        ":TCU",
        ":IR",
        ":ALU",
        ":ProcessorStatus"
    ],
)

verilator_cc_library(
    name = "PaletteLookupRGB",
    srcs = ["ppu/PaletteLookupRGB.v"],
)

gtest_verilog_testbench(
    name = "PaletteLookupRGBTestBench",
    deps = [":PaletteLookupRGB"]
)

verilator_cc_library(
    name = "PPU",
    srcs = ["ppu/PPU.v"],
)

gtest_verilog_testbench(
    name = "PPUTestBench",
    deps = [":PPU"]
)

verilator_cc_library(
    name = "PPUChipEnable",
    srcs = ["ppu/PPUChipEnable.v"]
)

gtest_verilog_testbench(
    name = "PPUChipEnableTestBench",
    deps = [":PPUChipEnable"]
)

verilator_cc_library(
    name = "Shift16",
    srcs = ["ppu/Shift16.v"],
)

gtest_verilog_testbench(
    name = "Shift16TestBench",
    deps = [":Shift16"]
)

# Generate instance of VGAGenerator for
#  a smaller screen (easier to test!)
verilator_cc_library(
    name = "VGAGenerator3x2",
    mtop = "VGAGenerator",
    srcs = ["ppu/vga/VGAGenerator.v"],
    vopts = [
        "-Wall",
        "-GWIDTH=11'd6",                # note: specifying bit-width of constants
        "-GHEIGHT=11'd3",
        "-GWIDTH_VISIBLE=11'd3",
        "-GHEIGHT_VISIBLE=11'd2"
    ]
)

gtest_verilog_testbench(
    name = "VGAGenerator3x2TestBench",
    deps = [":VGAGenerator3x2"]
)

# Generate instance of VGAOutput for
#  a smaller screen (easier to test!)
verilator_cc_library(
    name = "VGAOutput3x2",
    mtop = "VGAOutput",
    srcs = ["ppu/vga/VGAOutput.v"],
    vopts = [
        "-Wall",
        "-GWIDTH=11'd7",                # note: specifying bit-width of constants
        "-GHEIGHT=11'd5",
        "-GWIDTH_VISIBLE=11'd3",
        "-GHEIGHT_VISIBLE=11'd2",
        "-GFRONT_PORCH_X=11'd1",
        "-GBACK_PORCH_X=11'd1",
        "-GFRONT_PORCH_Y=11'd1",
        "-GBACK_PORCH_Y=11'd1",
    ]
)

gtest_verilog_testbench(
    name = "VGAOutput3x2TestBench",
    deps = [":VGAOutput3x2"]
)

cc_test(
    name = "test-ppu",
    srcs = glob(
        include =[
            "**/*.cpp",
            "**/*.h",
            "**/*.hpp",
            "**/*.inl"
        ],
        exclude = [
            "emulator/**/*",
            "cpu6502/**/*",
            "nes/**/*"
        ]
    ) + [
        ":PaletteLookupRGBTestBench",
        ":PPUTestBench",
        ":PPUChipEnableTestBench",
        ":Shift16TestBench",
        ":VGAGenerator3x2TestBench",
        ":VGAOutput3x2TestBench"
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":PaletteLookupRGB",
        ":PPU",
        ":PPUChipEnable",
        ":Shift16",
        ":VGAGenerator3x2",
        ":VGAOutput3x2"
    ],
)

verilator_cc_library(
    name = "ClockEnable",
    srcs = ["nes/ClockEnable.v"],
)

gtest_verilog_testbench(
    name = "ClockEnableTestBench",
    deps = [":ClockEnable"]
)

verilator_cc_library(
    name = "MemoryController",
    srcs = ["nes/MemoryController.v"],
)

gtest_verilog_testbench(
    name = "MemoryControllerTestBench",
    deps = [":MemoryController"]
)

verilator_cc_library(
    name = "Cpu2A03",
    srcs = ["nes/Cpu2A03.v"],
)

gtest_verilog_testbench(
    name = "Cpu2A03TestBench",
    deps = [":Cpu2A03"]
)

cc_test(
    name = "test-nes",
    srcs = glob(
        include =[
            "**/*.cpp",
            "**/*.h",
            "**/*.hpp",
            "**/*.inl"
        ],
        exclude = [
            "emulator/**/*",
            "cpu6502/**/*",
            "ppu/**/*"
        ]
    ) + [
        ":ClockEnableTestBench",
        ":MemoryControllerTestBench",
        ":Cpu2A03TestBench"
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":ClockEnable",
        ":MemoryController",
        ":Cpu2A03"
    ],
)

cc_binary(
    name = "emulator",
    srcs = glob(
        include =[
            "**/*.cpp",
            "**/*.h",
            "**/*.hpp",
            "**/*.inl"
        ],
        exclude = [
            "**/test/**/*",
            "**/*.test.cpp",
            "emulator/EmulatorVGA.cpp"
        ]
    ) + [
        ":Cpu6502TestBench"
    ],
    deps = [
        "@gtestverilog//gtestverilog:lib",
        ":Cpu6502"
    ],
    linkopts = [
        "-framework OpenGL",
        "-framework GLUT"
    ]
)

verilator_cc_library(
    name = "VGAExample",
    srcs = [
        "ppu/vga/VGAExample.v",
        "ppu/vga/VGAOutput.v",
        "ppu/vga/VGAGenerator.v"
    ]
)

gtest_verilog_testbench(
    name = "VGAExampleTestBench",
    deps = [":VGAExample"]
)

cc_binary(
    name = "emulator-vga",
    srcs = glob(
        include =[
            "**/*.cpp",
            "**/*.h",
            "**/*.hpp",
            "**/*.inl"
        ],
        exclude = [
            "**/test/**/*",
            "**/*.test.cpp",
            "emulator/Emulator.cpp",
            "emulator/Renderer.cpp"
        ]
    ) + [
        ":VGAExampleTestBench",
    ],
    deps = [
        "@gtestverilog//gtestverilog:lib",
        ":VGAExample",
    ],
    linkopts = [
        "-framework OpenGL",
        "-framework GLUT"
    ]
)
