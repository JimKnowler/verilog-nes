module PPUChipEnable(
    input i_clk_en_cpu,
    input i_clk_en_ppu,
    input [15:0] i_address,

    output o_ce
);

reg r_ce;

always @(*)
begin
    r_ce = i_clk_en_cpu 
        && i_clk_en_ppu
        && (i_address >= 16'd4000) && (i_address < 16'd5000);
end

assign o_ce = r_ce;

endmodule
