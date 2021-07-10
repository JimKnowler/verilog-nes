module PPUPatternTableAddress(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [14:0] i_v,
    output [14:0] o_address
);

// https://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching

reg [14:0] r_v;

always @(*)
begin
    r_v = 15'h2000 | (i_v & 15'h0FFF);
end

assign o_address = r_v;

endmodule