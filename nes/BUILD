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
    srcs = ["Cpu6502.v", "cpu6502/Decoder.v"],
)

gtest_verilog_testbench(
    name = "Cpu6502TestBench",
    deps = [":Cpu6502"]
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
        ":Cpu6502TestBench"
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":MasterClock",
        ":MemoryController",
        ":Cpu6502"
    ],
)
