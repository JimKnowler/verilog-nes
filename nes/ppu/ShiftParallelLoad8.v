// 8 bit shift register
// - parallet load
// - serial shift out

module ShiftParallelLoad8(
    input i_clk,
    input i_reset_n,

    input i_ce,

    input i_load,               // control - load value from i_data into most significant bit
    input [7:0] i_data,         // input data - value to load into register

    input i_shift,              // control - left shift on next clock pulse
    output o_shift_data,        // output data - value of offset bit in the shift register

    output [7:0] o_debug_data   // debug - current value in the shift register

);

reg [7:0] r_data;

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_data <= 0;
    end
    else if (i_ce)
    begin
        if (i_load)
            r_data <= i_data;
        else if (i_shift)
        begin
            r_data[7:1] <= r_data[6:0];
            r_data[0] <= 0;
        end
    end
end

assign o_debug_data = r_data;
assign o_shift_data = r_data[7];

endmodule
