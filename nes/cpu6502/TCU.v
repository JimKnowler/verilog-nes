// Timing Control Unit

module TCU(
    input i_clk,
    input i_reset_n,
    input [2:0] i_tcu_next,
    output [2:0] o_tcu
);

reg [2:0] r_tcu;

always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
        r_tcu <= 0;
    else
        r_tcu <= i_tcu_next;
end

assign o_tcu = r_tcu;

endmodule
