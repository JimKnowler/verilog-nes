// Timing Control Unit

module TCU(
    input i_clk,
    input i_reset_n,
    input [3:0] i_tcu_next,
    output [3:0] o_tcu,
    output o_sync
);

reg [3:0] r_tcu;

always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        // start at -1, so TCU starts at 0 on first tick
        r_tcu <= 4'b1111;
    end
    else
    begin
        r_tcu <= i_tcu_next;
    end
end

assign o_tcu = r_tcu;
assign o_sync = (r_tcu == 1);

endmodule
