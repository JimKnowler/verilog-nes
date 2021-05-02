// Input Data Latch
//
// Implemented as Transparent Latch, controlled by phi2
// - pass through while clock is HI
// - latch value on FALLING EDGE

module DL(
    input i_clk,
    input i_reset_n,
    
    input i_clk_en,             // clock enable

    input [7:0] i_data,
    output [7:0] o_data
);

reg [7:0] r_data;

always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_data <= 0;
    end
    else if (i_clk_en)
    begin
        r_data <= i_data;
    end
end

assign o_data = (i_clk && i_clk_en) ? i_data : r_data;

endmodule

