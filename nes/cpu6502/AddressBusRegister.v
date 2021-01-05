// Address Bus Register
// Transparent Latch
// - pass through during phi1
// - latch value at falling edge of phi1

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

always @(posedge i_clk)
begin
    if (i_load)
    begin
        r_address <= i_address;
    end
end

wire w_enabled = (i_clk == 0) && (i_load);

assign o_address = w_enabled ? i_address : r_address;

endmodule

