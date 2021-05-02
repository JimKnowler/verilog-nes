module PPUChipEnable(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input i_clk_en_cpu,
    input i_clk_en_ppu,
    input [15:0] i_address,         // Address that CPU is accessing

    output o_ce                     // PPU Chip enable
);

reg r_ce;

always @(*)
begin
    r_ce = (i_clk_en_cpu == 1)
        && (i_clk_en_ppu == 1)
        && (i_address >= 16'h2000) && (i_address < 16'h3000);
end

assign o_ce = r_ce;

endmodule
