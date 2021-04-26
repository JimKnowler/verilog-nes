module ClockEnable
(
    input i_clk,                    // input clock - PPU
    input i_reset_n,
    output o_ce_cpu                 // output clock enable for CPU
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
        if (r_counter == 2)
            r_counter <= 0;
        else
            r_counter <= r_counter + 1;
    end
end

assign o_ce_cpu = (r_counter == 2);

endmodule
