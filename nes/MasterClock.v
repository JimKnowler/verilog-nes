module MasterClock(
    input i_clk,
    input i_reset_n
);

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
    end
    else
    begin
    end
end


endmodule