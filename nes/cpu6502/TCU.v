// Timing Control Unit

module TCU(
    input i_clk,
    input i_reset_n,
    input [2:0] i_tcu_next,
    output [2:0] o_tcu,
    output o_sync
);

reg [2:0] r_tcu;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_tcu <= 0;
    end
    else
    begin
        r_tcu <= i_tcu_next;
    end
end

assign o_tcu = r_tcu;
assign o_sync = (r_tcu == 1);

endmodule
