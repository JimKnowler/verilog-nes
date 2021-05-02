// Data Output Register
// - including the data bus tristate buffer
// - Where DOR is transparent latch on phi1
// - Where Tristate bus is enabled on phi2
// - implemented as register that latches in on rising edge of phi2

module DOR(
    input i_clk,
    input i_reset_n,

    input i_clk_en,                 // clock enable

    input [7:0] i_data,         // data input from DB bus
    input i_rw,                 // external read/write pin 
                                // (read = HIGH, write = LOW)
    output [7:0] o_data         // data output from 6502
);

reg [7:0] r_data;
reg r_tristate_enable;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_data <= 8'h00;
        r_tristate_enable <= 0;
    end
    else if (i_clk && i_clk_en)
    begin
        // phi2 rising edge
        r_tristate_enable <= !i_rw;
        r_data <= i_data;
    end
end

assign o_data = (r_tristate_enable) ? r_data : 0;

endmodule
