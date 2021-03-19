module MasterClock(
    input i_clk,
    input i_reset_n,
    output o_clk_cpu,
    output o_clk_ppu,
    output o_clk_vga,
    output o_clk_mcu
);

reg r_clk_cpu;
reg r_clk_ppu;
reg r_clk_vga;
reg r_clk_mcu;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_clk_cpu <= 0;
        r_clk_ppu <= 0;
        r_clk_vga <= 0;
        r_clk_mcu <= 0;

        // todo: reset internal counters
    end
    else
    begin
        // todo: increment internal counters
    end
end

assign o_clk_cpu = r_clk_cpu;
assign o_clk_ppu = r_clk_ppu;
assign o_clk_vga = r_clk_vga;
assign o_clk_mcu = r_clk_mcu;

endmodule