// 8 bit shift register
// - serial shift in
// - parallel out

module Shift8(
    input i_clk,
    input i_reset_n,

    input i_ce,

    input i_load,               // control - load value from i_data into most significant bit
    input i_data,               // input data - value to load into most-significant bit

    input i_shift,              // control - left shift on next clock pulse
    input [2:0] i_offset,       // offset into top 8 bits of shift register
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
        if (i_shift)
        begin
            r_data[6:0] <= r_data[7:1];

            if (i_load)
            begin
                r_data[7] <= i_data;
            end
            else
            begin    
                r_data[0] <= 0;
            end
        end
        else if (i_load)
        begin
            r_data[7] <= i_data;
        end
    end
end

assign o_debug_data = r_data;
assign o_shift_data = r_data[i_offset];

endmodule