// Timing Control Unit

module TCU(
    input i_clk,
    input i_reset_n,
    input [2:0] i_tcu_next,
    output [2:0] o_tcu,
    output o_sync
);

reg [2:0] r_tcu;
reg r_started;

always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_tcu <= 0;
        r_started <= 0;
    end
    else
    begin
        if (r_started)
            r_tcu <= i_tcu_next;
        else
            r_started <= 1;
    end
end

assign o_tcu = r_tcu;
assign o_sync = (r_tcu == 1);

endmodule
