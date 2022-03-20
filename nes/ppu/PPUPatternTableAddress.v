module PPUPatternTableAddress(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [2:0] i_t,                // fine Y offset, the row number within a tile
    input i_p,                      // bit plane: 0=lower, 1=upper
    input [3:0] i_c,                // tile column (=low nibble of nametable entry)
    input [3:0] i_r,                // tile row (=high nibble of nametable entry)
    input i_h,                      // Half of the sprite table: 0=left, 1=right (from PPUCTRL)

    output [13:0] o_address
);

// https://wiki.nesdev.com/w/index.php/PPU_pattern_tables

reg [13:0] r_address;

always @(*)
begin
    r_address = {1'b0, i_h, i_r, i_c, i_p, i_t};
end

assign o_address = r_address;

endmodule
