// Register
// Implementation of S, AC, X and Y registers
module Register
#(
    // NOTE: 0xFF is default value used for Stack
    parameter [7:0] DEFAULT_VALUE = 8'hFF
)
(
    input i_clk,
    input i_reset_n,

    input [7:0] i_data,         // input from data bus
    input i_load,               // control signal to load
    output [7:0] o_data         // value stored in register
);

/// assumption: not clear from 6502 diagram when
///      register is loaded, so assuming that value 
///      is loaded on falling edge of clk while i_load
//       is high

reg [7:0] r_data;

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
        r_data <= DEFAULT_VALUE;
    else if (i_load)
        r_data <= i_data;
end

assign o_data = r_data;

endmodule