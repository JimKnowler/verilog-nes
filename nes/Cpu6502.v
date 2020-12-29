module Cpu6502(
    input i_clk,
    input i_reset_n,

    /* verilator lint_off UNDRIVEN */
    /* verilator lint_off UNUSED */
    output o_rw,            // Read / Write - where 1 = READ, 0 = WRITE
    output [15:0] o_address,
    input [7:0] i_data,
    output o_data
    /* verilator lint_on UNDRIVEN */
    /* verilator lint_on UNUSED */
);

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        // todo: reset behaviour
    end
    else
    begin
        // todo: get on with it
    end
end


endmodule
