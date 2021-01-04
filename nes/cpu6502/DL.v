// Input Data Latch

module DL(
    input i_clk,
    /* verilator lint_off UNUSED */
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [7:0] i_data,
    output [7:0] o_data
);

reg [7:0] r_data;

// phi1: when read data is valid on data bus
always @(negedge i_clk)
begin
    r_data <= i_data;
end

assign o_data = r_data;

endmodule

