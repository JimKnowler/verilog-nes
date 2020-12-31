// Register
// Implementation of S, AC, X and Y registers
module Register(
    input i_clk,
    /* verilator lint_off UNUSED */
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [7:0] i_data,         // input from data bus
    input i_load,               // control signal to load
    output [7:0] o_data         // value stored in register
);

/// assumption: that value is loaded on falling edge of clk

reg [7:0] r_data;

always @(negedge i_clk)
begin
    if (i_load)
        r_data <= i_data;
end

assign o_data = r_data;

endmodule