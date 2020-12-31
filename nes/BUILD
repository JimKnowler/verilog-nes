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
        ":PCLTestBench",
        ":AddressBusRegisterTestBench",
        ":DLTestBench",
        ":RoutingTestBench",
        ":RegisterTestBench",
        ":DORTestBench"
    ],
    deps = [
        "@com_google_googletest//:gtest",
        "@gtestverilog//gtestverilog:lib",
        ":MasterClock",
        ":MemoryController",
        ":Cpu6502",
        ":PCH",
        ":PCL",
        ":AddressBusRegister",
        ":DL",
        ":Routing",
        ":Register",
        ":DOR"
    ],
)
