module Cpu6502(
    input i_clk,
    input i_reset_n,

    output o_rw,                            // Read / Write - where 1 = READ, 0 = WRITE
    output [15:0] o_address,
    input [7:0] i_data,                     // 8 bit data - used for READ
    output [7:0] o_data,                    // 8 bit data - used for WRITE

    ///////////////////////////////////
    // debug output
    
    output [7:0] o_debug_tcu,               // Timing Control Unit
    output [15:0] o_debug_pc,               // Program Counter
    output [7:0] o_debug_ir,                // Instruction Register
    output [7:0] o_debug_state,             // State
    output [7:0] o_debug_a                  // value in A register
    // todo: registers a,x,y,alu
);

localparam STATE_RESET_VECTOR = 0;          // read two bytes from the reset vector
                                            //    and load as address into PC
localparam STATE_EXECUTE_OPCODES = 1;       // load opcodes from address in PC & execute


localparam ADDRESS_RESET_VECTOR = 16'hFFFC;

localparam RW_READ = 1;
localparam RW_WRITE = 0;

localparam OPCODE_NOP = 8'hEA;
localparam OPCODE_LDAi = 8'hA9;
localparam OPCODE_STAa = 8'h8D;

localparam ADDRESS_MODE_PC = 1'b0;          // use r_pc to drive o_address
localparam ADDRESS_MODE_ALT = 1'b1;         // use r_address to drive o_address

reg [7:0] r_state;
reg [7:0] r_tcu;                            // Timing Control Unit - track current stage of current opcode
reg [15:0] r_pc;                            // Program Counter
reg r_rw;                                   // Read / Write
reg [15:0] r_address;                       // Register that drives address for data write & 
                                            //   reset / irq / nmi vectors, 
reg r_address_mode;                         // Whether to use r_address or r_pc for o_address
reg [7:0] r_ir;                             // Instruction Register
reg [7:0] r_a;                              // 'A' data register
reg [7:0] r_data;                           // Data writen to o_data

always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_tcu <= 0;
        r_rw <= RW_READ;
        r_state <= STATE_RESET_VECTOR;
        r_address <= 0;
        r_address_mode <= ADDRESS_MODE_ALT;
    end
    else
    begin
        r_tcu <= r_tcu + 1;
        
        if (r_state == STATE_RESET_VECTOR)
        begin
            // falling edge
            if (r_tcu == 0)
            begin
                r_address <= ADDRESS_RESET_VECTOR;
            end
            if (r_tcu == 1)
            begin
                r_pc[7:0] <= i_data;
                r_address <= r_address + 1;
            end
            else if (r_tcu == 2)
            begin
                r_pc[15:8] <= i_data;
                r_state <= STATE_EXECUTE_OPCODES;
                r_address_mode <= ADDRESS_MODE_PC;
                r_tcu <= 1;
            end
        end

        if (r_state == STATE_EXECUTE_OPCODES)
        begin
            if (r_tcu == 0)
            begin
                // fetching opcode
            end
            else if (r_tcu == 1)
            begin
                r_ir <= i_data;
                r_pc <= r_pc + 1;
            end
            else if (r_tcu == 2)
            begin
                if (r_ir != OPCODE_NOP)
                begin
                    r_pc <= r_pc + 1;
                end

                if (r_ir == OPCODE_LDAi)
                begin
                    r_a <= i_data;
                end

                if (r_ir == OPCODE_STAa)
                begin
                    r_address[7:0] <= i_data;
                end

                if (r_ir != OPCODE_STAa)
                begin
                    r_tcu <= 1;
                end
            end
            else if (r_tcu == 3)
            begin
                if (r_ir == OPCODE_STAa)
                begin
                    r_address[15:8] <= i_data;
                    r_address_mode <= ADDRESS_MODE_ALT;
                    r_data <= r_a;
                    r_rw <= 0;
                end
            end
            else if (r_tcu == 4)
            begin
                if (r_ir == OPCODE_STAa)
                begin
                    r_address_mode <= ADDRESS_MODE_PC;
                    r_rw <= 1;
                    r_data <= 0;
                    r_pc <= r_pc + 1;
                    r_tcu <= 1;
                end
            end
        end
    end
end

assign o_debug_tcu = r_tcu;
assign o_debug_pc = r_pc;
assign o_debug_ir = r_ir;
assign o_debug_state = r_state;
assign o_debug_a = r_a;

assign o_address = (r_address_mode == ADDRESS_MODE_PC) ? r_pc : r_address;
assign o_rw = r_rw;
assign o_data = r_data;

endmodule
