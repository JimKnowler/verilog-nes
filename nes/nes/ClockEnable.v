module ClockEnable
(
    input i_clk,                    // input clock - PPU
    input i_reset_n,
    input i_ce,                     // input clock enable

    output o_ce_cpu,                // output clock enable for CPU
    output o_ce_ppu                 // output clock enable for PPU
);

reg [1:0] r_counter;

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_counter <= -1;
    end
    else
    begin
        if (i_ce)
            if (r_counter == 2)
                r_counter <= 0;
            else
                r_counter <= r_counter + 1;
    end
end

assign o_ce_cpu = i_ce && (r_counter == 2);
assign o_ce_ppu = i_ce;

endmodule
