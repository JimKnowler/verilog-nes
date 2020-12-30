load("@rules_verilator//verilator:defs.bzl", "verilator_cc_library")
load("@gtestverilog//gtestverilog:defs.bzl", "gtest_verilog_testbench")

verilator_cc_library(
    name = "MasterClock",
    srcs = ["MasterClock.v"],
)

gtest_verilog_testbench(
    name = "MasterClockTestBench",
    deps = [":MasterClock"]
)

verilator_cc_library(
    name = "MemoryController",
    srcs = ["MemoryController.v"],
)

gtest_verilog_testbench(
    name = "MemoryControllerTestBench",
    deps = [":MemoryController"]
)

verilator_cc_library(
    name = "Cpu6502",
    srcs = [
        "Cpu6502.v", 
        "cpu6502/Decoder.v",
        "cpu6502/PCL.v",
        "cpu6502/PCH.v"
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

cc_test(
    name = "test",
    srcs = glob(
        include =[
            "**/*.cpp",
            "**/*.h"
        ]
    ) + [
        ":MasterClockTestBench",
        ":MemoryControllerTestBench",
        ":Cpu6502TestBench",
        ":PCHTestBench",
        ":PCLTestBench"
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":MasterClock",
        ":MemoryController",
        ":Cpu6502",
        ":PCH",
        ":PCL"
    ],
)
