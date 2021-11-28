// Timing Control Unit

module TCU(
    input i_clk,
    input i_reset_n,

    input i_clk_en,

    input [3:0] i_tcu_next,
    output [3:0] o_tcu,
    output o_sync
);

reg [3:0] r_tcu;
reg r_sync;

always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        // start at -1, so TCU starts at 0 on first tick
        r_tcu <= 4'b1111;
        r_sync <= 0;
    end
    else if (i_clk_en)
    begin
        r_tcu <= i_tcu_next;
        r_sync <= (i_tcu_next == 0);
    end
end

assign o_tcu = r_tcu;
assign o_sync = r_sync;

endmodule
