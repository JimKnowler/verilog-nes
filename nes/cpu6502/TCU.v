// Timing Control Unit

module TCU(
    input i_clk,
    input i_reset_n,
    input [2:0] i_tcu_next,
    output [2:0] o_tcu,
    output o_sync
);

reg [2:0] r_tcu;
reg r_sync;

always @(posedge i_clk or negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_tcu <= 0;
        r_sync <= 1;
    end
    else
    begin
        if (i_clk)
            r_tcu <= i_tcu_next;
        
        r_sync <= (r_tcu == 1);
    end
end

assign o_tcu = r_tcu;
assign o_sync = r_sync;

endmodule
