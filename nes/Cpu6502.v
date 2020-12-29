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
    output [15:0] o_debug_pc,
    output [7:0] o_debug_state
);

localparam STATE_IDLE = 0;
localparam STATE_RESET_VECTOR = 1;                  // read two bytes from the reset vector

localparam ADDRESS_RESET_VECTOR = 16'hFFFC;

localparam RW_READ = 1;
localparam RW_WRITE = 0;

reg [7:0] r_state;
reg [7:0] r_tcu;
reg [15:0] r_pc;
reg r_rw;
reg [15:0] r_address_vector;

always @(posedge i_clk or negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_tcu <= 0;
        r_rw <= RW_READ;
        r_state <= STATE_RESET_VECTOR;
        r_address_vector <= 0;
    end
    else
    begin
        if (i_clk)
        begin
            // rising edge
            
        end
        else 
        begin
            r_tcu <= r_tcu + 1;
            
            // falling edge
            if (r_tcu == 0)
            begin
                r_address_vector <= ADDRESS_RESET_VECTOR;
            end
            if (r_tcu == 1)
            begin
                r_pc[7:0] <= i_data;
                r_address_vector <= r_address_vector + 1;
            end
            else if (r_tcu == 2)
            begin
                r_pc[15:8] <= i_data;
                r_state <= STATE_IDLE;
            end
        end
    end
end


assign o_debug_tcu = r_tcu;
assign o_debug_pc = r_pc;
assign o_debug_state = r_state;

assign o_address = (r_state == STATE_RESET_VECTOR) ? r_address_vector : r_pc;
assign o_rw = r_rw;

endmodule
