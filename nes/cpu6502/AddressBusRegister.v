module AddressBusRegister(
    input i_clk,
    /* verilator lint_off UNUSED */
    input i_reset_n,
    /* verilator lint_off UNUSED */

    input [7:0] i_address,
    input i_load,
    output [7:0] o_address
);

reg [7:0] r_address;

always @(negedge i_clk)
begin
    if (i_load)
    begin
        r_address <= i_address;
    end
end

assign o_address = r_address;

endmodule

