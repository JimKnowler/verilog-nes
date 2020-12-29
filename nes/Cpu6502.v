module Cpu6502(
    input i_clk,
    input i_reset_n,

    /* verilator lint_off UNDRIVEN */
    /* verilator lint_off UNUSED */
    output o_rw,            // Read / Write - where 1 = READ, 0 = WRITE
    output [15:0] o_address,
    input [7:0] i_data,
    output [7:0] o_data,
    /* verilator lint_on UNDRIVEN */
    /* verilator lint_on UNUSED */

    // debug output
    output [7:0] o_debug_tcu,
    output [15:0] o_debug_pc
);

localparam STATE_IDLE = 0;
localparam STATE_RESET_VECTOR = 1;

localparam ADDRESS_RESET_VECTOR = 16'hFFFC;

reg [7:0] r_tcu;
reg [15:0] r_pc;

always @(posedge i_clk or negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_tcu <= 0;
    end
    else
    begin
        if (i_clk)
        begin
            // rising edge
            // todo: only increment r_tcu if previous falling-edge has executed
            //      - test by resetting and then immediately giving clk a rising edge 
            r_tcu <= r_tcu + 1;
        end
        else 
        begin
            // falling edge
            if (r_tcu == 0)
            begin
                // note: pc is probably the wrong place to put this..
                //      we're reading an address from reset vector, not opcodes
                r_pc <= ADDRESS_RESET_VECTOR;
            end
            if (r_tcu == 1)
            begin
                r_pc <= r_pc + 1;
            end
        end
    end
end


assign o_debug_tcu = r_tcu;
assign o_debug_pc = r_pc;
assign o_address = r_pc;

endmodule
