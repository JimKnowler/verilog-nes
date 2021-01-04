// Input Data Latch
//
// Implemented as Transparent Latch, controlled by phi2
// - pass through while clock is HIGH
// - latch value on FALLING EDGE

module DL(
    input i_clk,
    /* verilator lint_off UNUSED */
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [7:0] i_data,
    output [7:0] o_data
);

reg [7:0] r_data;


always @(negedge i_clk)
begin
    r_data <= i_data;
end

assign o_data = i_clk ? i_data : r_data;

endmodule

