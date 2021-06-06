module PPUMemoryMap(
    input i_clk,
    input i_reset_n
);

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
    end
    else
    begin
    end
end

endmodule
