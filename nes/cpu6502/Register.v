// Register
// Implementation of S, AC, X and Y registers
//
// 6502 uses Transparent Latch Registers
// - Pass input through while i_load is HIGH
// - Latch input on FALLING EDGE of i_load 
// Reference: http://forum.6502.org/viewtopic.php?f=4&t=4651
//
// This Register simulation will only clock values 
// at falling edge of i_load

module Register
#(
    // NOTE: 0xFF is default value used for Stack
    parameter [7:0] DEFAULT_VALUE = 8'hFF
)
(
    /* verilator lint_off UNUSED */
    input i_clk,
    /* verilator lint_on UNUSED */
    input i_reset_n,

    input [7:0] i_data,         // input from data bus
    input i_load,               // control signal to load
    output [7:0] o_data         // value stored in register
);

reg [7:0] r_data;

always @(negedge i_reset_n or negedge i_load)
begin
    if (!i_reset_n)
        r_data <= DEFAULT_VALUE;
    else
        r_data <= i_data;
end

assign o_data = r_data;

endmodule