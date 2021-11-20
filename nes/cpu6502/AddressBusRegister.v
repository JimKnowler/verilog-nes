// Address Bus Register
// Transparent Latch
// - pass through during phi1
// - latch value at falling edge of phi1

module AddressBusRegister(
    input i_clk,
    input i_reset_n,

    input i_clk_en,             // clock enable

    input [7:0] i_address,
    input i_load,
    output [7:0] o_address
);

reg [7:0] r_address;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_address <= 0;
    end
    else
    begin
        if (i_load && i_clk_en)
        begin
            r_address <= i_address;
        end
    end
end

wire w_enabled = (i_clk == 0) && i_load && i_clk_en;

assign o_address = w_enabled ? i_address : r_address;

endmodule

