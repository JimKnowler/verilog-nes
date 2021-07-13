module PPUAttributeAddress(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [14:0] i_v,
    output [14:0] o_address
);

// https://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching

reg [14:0] r_address;

always @(*)
begin
    r_address = 15'h23C0 | (i_v & 15'h0C00) | { {9{1'b0}}, i_v[9:7], {3{1'b0}}} | { {12{1'b0}}, i_v[4:2] };
end

assign o_address = r_address;

endmodule
