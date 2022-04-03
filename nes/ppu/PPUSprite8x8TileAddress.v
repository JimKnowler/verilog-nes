module PPUSprite8x8TileAddress(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [2:0] i_fine_y_offset,          // row number within the 8x8 tile
    input i_bit_plane,
    input [7:0] i_tile,
    input i_ppuctrl_s,
    
    output [13:0] o_address
);

reg [13:0] r_address;

always @(*)
begin
    // based on: https://www.nesdev.org/wiki/PPU_pattern_tables
    r_address = { 1'b0, i_ppuctrl_s, i_tile, i_bit_plane, i_fine_y_offset};
end

assign o_address = r_address;

endmodule
