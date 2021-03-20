module ClockDivider
#(
    DIVIDER_PPU = 4,
    DIVIDER_CPU = 12,
    DIVIDER_MCU = 2
)
(
    input i_clk,
    input i_reset_n,
    output o_clk_cpu,
    output o_clk_ppu,
    output o_cs_n_ppu,
    output o_clk_mcu            // todo: what speed should this run at?  multiple of PPU?
);

reg r_clk_cpu;
reg r_clk_ppu;
reg r_cs_n_ppu;
reg r_clk_mcu;

reg [15:0] r_counter;

always @(posedge i_clk or negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_clk_ppu <= 0;
        r_cs_n_ppu <= 0;
        r_clk_mcu <= 0;

        r_counter <= 0;
    end
    else
    begin
        if (!i_clk)
        begin
            if (r_counter >= (DIVIDER_CPU-1))
                r_counter <= 0;   
            else
                r_counter <= r_counter + 1;        

            r_clk_cpu <= (r_counter >= (DIVIDER_CPU/2)) ? 1 : 0;
            r_clk_ppu <= ((r_counter % DIVIDER_PPU) >= (DIVIDER_PPU/2)) ? 1 : 0;
            r_clk_mcu <= ((r_counter % DIVIDER_MCU) >= (DIVIDER_MCU/2)) ? 1 : 0;
        end
        else
        begin
            r_cs_n_ppu <= (r_counter == 0) ? 0 : 1;
        end
    end
end

assign o_clk_cpu = r_clk_cpu;
assign o_clk_ppu = r_clk_ppu;
assign o_cs_n_ppu = r_cs_n_ppu;
assign o_clk_mcu = r_clk_mcu;

endmodule