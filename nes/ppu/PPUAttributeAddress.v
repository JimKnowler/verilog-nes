module PPUAttributeAddress(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [13:0] i_v,
    output [13:0] o_address
);

// https://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching

reg [13:0] r_address;

always @(*)
begin
    r_address = 14'h23C0 | (i_v[13:0] & 14'h0C00) | { {8{1'b0}}, i_v[9:7], {3{1'b0}}} | { {11{1'b0}}, i_v[4:2] };
end

assign o_address = r_address;

endmodule
