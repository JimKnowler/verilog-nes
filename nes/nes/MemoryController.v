module MemoryController(
    input i_clk,                    // pixel clock
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
        //    1. PPU
        //    2. CPU
        //    3. DEBUG
        // - execute the currently active request
    end
end

endmodule;
