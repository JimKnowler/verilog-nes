module MemoryController(
    input i_clk,
    input i_reset_n

    // cpu r/w
    // ppu r
    // debug r/w
);

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        // todo: reset
    end
    else
    begin
        // - check active request lines
        // - prioritise requests
        // - execute the currently active request
    end
end

endmodule;
