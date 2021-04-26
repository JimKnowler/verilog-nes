module Shift16(
    input i_clk,
    input i_reset_n,

    input i_load,               // control - load value from i_data into low byte
    input [7:0] i_data,         // data - data to load into low byte
    

    input i_shift,              // control - left shift on next clock pulse
    input [3:0] i_offset,       // offset into top 8 bits of shift register
    output o_shift_data,        // data - value of top bit in the shift register

    output [15:0] o_debug_data  // debug - current value in the shift register

);

reg [15:0] r_data;
wire [3:0] w_shift_offset;

always @(negedge i_reset_n or posedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_data <= 0;
    end
    else
    begin
        if (i_shift)
        begin
            if (i_load)
            begin
                r_data[15:8] <= r_data[14:7];
                r_data[7:0] <= i_data;
            end
            else
            begin
                r_data[15:1] <= r_data[14:0];
                r_data[0] <= 0;
            end
        end
        else if (i_load)
        begin
            r_data[7:0] <= i_data;
        end
    end
end


assign o_debug_data = r_data;
assign w_shift_offset = 4'd15 - i_offset;
assign o_shift_data = r_data[w_shift_offset];

endmodule