load("@rules_verilator//verilator:defs.bzl", "verilator_cc_library")
load("@gtestverilog//gtestverilog:defs.bzl", "gtest_verilog_testbench")

cpu6502_srcs = [
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
]

ppu_srcs = [
    "ppu/PPU.v",
    "ppu/PaletteLookupRGB.v",
    "ppu/PPUAttributeAddress.v",
    "ppu/PPUIncrementX.v",
    "ppu/PPUIncrementY.v",
    "ppu/PPUTileAddress.v",
    "ppu/PPUPatternTableAddress.v",
    "ppu/Shift8.v",
    "ppu/Shift16.v"
]

nes_srcs = [
    "nes/NES.v", 
    "nes/Cpu2A03.v",
    "nes/ClockEnable.v",
    "nes/CPUMemoryMap.v",
    "nes/PPUMemoryMap.v",
    "ppu/PPUChipEnable.v"
] + cpu6502_srcs + ppu_srcs

verilator_cc_library(
    name = "Cpu6502",
    srcs = cpu6502_srcs,
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
            "nes/**/*",
            "debugger-cpu/**/*",
            "debugger-nes/**/*",
            "debugger-common/**/*"
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
    srcs = ppu_srcs,
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

verilator_cc_library(
    name = "Shift8",
    srcs = ["ppu/Shift8.v"],
)

gtest_verilog_testbench(
    name = "Shift8TestBench",
    deps = [":Shift8"]
)

verilator_cc_library(
    name = "PPUAttributeAddress",
    srcs = ["ppu/PPUAttributeAddress.v"],
)

gtest_verilog_testbench(
    name = "PPUAttributeAddressTestBench",
    deps = [":PPUAttributeAddress"]
)

verilator_cc_library(
    name = "PPUIncrementX",
    srcs = ["ppu/PPUIncrementX.v"],
)

gtest_verilog_testbench(
    name = "PPUIncrementXTestBench",
    deps = [":PPUIncrementX"]
)

verilator_cc_library(
    name = "PPUIncrementY",
    srcs = ["ppu/PPUIncrementY.v"],
)

gtest_verilog_testbench(
    name = "PPUIncrementYTestBench",
    deps = [":PPUIncrementY"]
)

verilator_cc_library(
    name = "PPUTileAddress",
    srcs = ["ppu/PPUTileAddress.v"],
)

gtest_verilog_testbench(
    name = "PPUTileAddressTestBench",
    deps = [":PPUTileAddress"]
)

verilator_cc_library(
    name = "PPUPatternTableAddress",
    srcs = ["ppu/PPUPatternTableAddress.v"],
)

gtest_verilog_testbench(
    name = "PPUPatternTableAddressTestBench",
    deps = [":PPUPatternTableAddress"]
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
            "nes/**/*",
            "debugger-cpu/**/*",
            "debugger-nes/**/*",
            "debugger-common/**/*"
        ]
    ) + [
        ":PaletteLookupRGBTestBench",
        ":PPUTestBench",
        ":PPUChipEnableTestBench",
        ":Shift8TestBench",
        ":Shift16TestBench",
        ":VGAGenerator3x2TestBench",
        ":VGAOutput3x2TestBench",
        ":PPUAttributeAddressTestBench",
        ":PPUIncrementXTestBench",
        ":PPUIncrementYTestBench",
        ":PPUTileAddressTestBench",
        ":PPUPatternTableAddressTestBench"
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":PaletteLookupRGB",
        ":PPU",
        ":PPUChipEnable",
        ":Shift8",
        ":Shift16",
        ":VGAGenerator3x2",
        ":VGAOutput3x2",
        ":PPUAttributeAddress",
        ":PPUIncrementX",
        ":PPUIncrementY",
        ":PPUTileAddress",
        ":PPUPatternTableAddress"
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
    name = "Cpu2A03",
    srcs = ["nes/Cpu2A03.v"] + cpu6502_srcs
)

gtest_verilog_testbench(
    name = "Cpu2A03TestBench",
    deps = [":Cpu2A03"]
)

verilator_cc_library(
    name = "CPUMemoryMap",
    srcs = ["nes/CPUMemoryMap.v"],
)

gtest_verilog_testbench(
    name = "CPUMemoryMapTestBench",
    deps = [":CPUMemoryMap"]
)

verilator_cc_library(
    name = "PPUMemoryMap",
    srcs = ["nes/PPUMemoryMap.v"],
)

gtest_verilog_testbench(
    name = "PPUMemoryMapTestBench",
    deps = [":PPUMemoryMap"]
)

verilator_cc_library(
    name = "NES",
    srcs = nes_srcs
)

gtest_verilog_testbench(
    name = "NESTestBench",
    deps = [":NES"]
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
            "cpu6502/test/**/*",
            "ppu/**/*",
            "debugger-cpu/**/*",
            "debugger-nes/**/*",
            "debugger-common/**/*"
        ]
    ) + [
        ":ClockEnableTestBench",
        ":MemoryControllerTestBench",
        ":Cpu2A03TestBench",
        ":PPUMemoryMapTestBench",
        ":CPUMemoryMapTestBench",
        ":NESTestBench"
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":ClockEnable",
        ":Cpu2A03",
        ":CPUMemoryMap",
        ":PPUMemoryMap",
        ":NES"
    ],
)

cc_binary(
    name = "emulator-cpu",
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
            "emulator/EmulatorVGA.cpp",
            "emulator/EmulatorNES.cpp",
            "debugger-cpu/**/*",
            "debugger-nes/**/*",
            "debugger-common/**/*"
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
            "emulator/EmulatorCPU.cpp",
            "emulator/EmulatorNES.cpp",
            "emulator/RendererCPU.cpp",
            "debugger-cpu/**/*",
            "debugger-nes/**/*",
            "debugger-common/**/*"
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

cc_binary(
    name = "emulator-nes",
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
            "emulator/EmulatorVGA.cpp",
            "emulator/EmulatorCPU.cpp",
            "emulator/RendererCPU.cpp",
            "debugger-cpu/**/*",
            "debugger-nes/**/*",
            "debugger-common/**/*"
        ]
    ) + [
        ":NESTestBench"
    ],
    deps = [
        "@gtestverilog//gtestverilog:lib",
        ":NES"
    ],
    linkopts = [
        "-framework OpenGL",
        "-framework GLUT"
    ]
)

#
# Debugger Common
#

verilator_cc_library(
    name = "Memory",
    srcs = ["debugger-common/simulation/Memory.v"]
)

gtest_verilog_testbench(
    name = "MemoryTestBench",
    deps = [":Memory"]
)

verilator_cc_library(
    name = "Sync",
    srcs = ["debugger-common/Sync.v"]
)

gtest_verilog_testbench(
    name = "SyncTestBench",
    deps = [":Sync"]
)

cc_test(
    name = "test-debugger-common",
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
            "ppu/**/*",
            "nes/**/*",
            "debugger-cpu/**/*",
            "debugger-nes/**/*"
        ]
    ) + [
        ":MemoryTestBench",
        ":SyncTestBench",
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":Memory",
        ":Sync"
    ],
)

#
# Debugger CPU
#

verilator_cc_library(
    name = "CPUDebugger",
    srcs = ["debugger-cpu/CPUDebugger.v"]
)

gtest_verilog_testbench(
    name = "CPUDebuggerTestBench",
    deps = [":CPUDebugger"]
)

verilator_cc_library(
    name = "CPUDebuggerMCU",
    srcs = ["debugger-cpu/CPUDebuggerMCU.v"]
)

gtest_verilog_testbench(
    name = "CPUDebuggerMCUTestBench",
    deps = [":CPUDebuggerMCU"]
)

verilator_cc_library(
    name = "CPUDebuggerTop",
    srcs = [
        "debugger-cpu/CPUDebuggerTop.v",
        "debugger-cpu/CPUDebugger.v",
        "debugger-cpu/CPUDebuggerMCU.v",
        "debugger-cpu/CPUDebuggerValues.v",
        "debugger-common/simulation/Memory.v",
        "debugger-common/SPIPeripheral.v",
        "debugger-common/Sync.v"        
    ] + cpu6502_srcs
)

gtest_verilog_testbench(
    name = "CPUDebuggerTopTestBench",
    deps = [":CPUDebuggerTop"]
)

cc_test(
    name = "test-debugger-cpu",
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
            "ppu/**/*",
            "nes/**/*",
            "debugger-nes/**/*",
            "debugger-common/**/*"
        ]
    ) + [
        ":CPUDebuggerTestBench",
        ":CPUDebuggerTopTestBench",
        ":CPUDebuggerMCUTestBench"
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":CPUDebugger",
        ":CPUDebuggerTop",
        ":CPUDebuggerMCU"
    ],
)

#
# Debugger NES
#

verilator_cc_library(
    name = "NESDebugger",
    srcs = ["debugger-nes/NESDebugger.v"]
)

gtest_verilog_testbench(
    name = "NESDebuggerTestBench",
    deps = [":NESDebugger"]
)

verilator_cc_library(
    name = "NESDebuggerMCU",
    srcs = ["debugger-nes/NESDebuggerMCU.v"]
)

gtest_verilog_testbench(
    name = "NESDebuggerMCUTestBench",
    deps = [":NESDebuggerMCU"]
)

verilator_cc_library(
    name = "NESDebuggerTop",
    srcs = [
        "debugger-nes/NESDebuggerTop.v",
        "debugger-nes/NESDebugger.v",
        "debugger-nes/NESDebuggerMCU.v",
        "debugger-nes/NESDebuggerValues.v",
        "debugger-common/simulation/Memory.v",
        "debugger-common/SPIPeripheral.v",
        "debugger-common/Sync.v"        
    ] + nes_srcs
)

gtest_verilog_testbench(
    name = "NESDebuggerTopTestBench",
    deps = [":NESDebuggerTop"]
)

cc_test(
    name = "test-debugger-nes",
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
            "ppu/**/*",
            "nes/**/*",
            "debugger-cpu/**/*",
            "debugger-common/**/*"
        ]
    ) + [
        ":NESDebuggerTestBench",
        ":NESDebuggerTopTestBench",
        ":NESDebuggerMCUTestBench"
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":NESDebugger",
        ":NESDebuggerTop",
        ":NESDebuggerMCU"
    ],
)