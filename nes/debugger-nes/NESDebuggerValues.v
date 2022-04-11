/*
 * Write: VALUEID_CPU_STEP = 1, to trigger a single step of CPU
 * Read: VALUEID_CPU_STEP, ==1 while stepping, ==0 when finished stepping
 * Read: VALUDID_CPU_xxxx to read state of CPU
 */

module NESDebuggerValues(
    input i_clk,
    input i_reset_n,

    input i_ena,
    input i_wea,
    input [15:0] i_id,
    input [15:0] i_data,
    output [15:0] o_data,
    
    // NES reset_n signal
    output o_nes_reset_n,

    // memory pool selector for debugger memory access
    output [1:0] o_debugger_memory_pool
);

// Set the value of RESET_N pin on the NES
localparam VALUEID_NES_RESET_N = 1;

// Set the memory pool that the debugger accesses
// 0 = PRG, 1 = RAM, 2 = PATTERNTABLE (CHR), 3 = NAMETABLE
localparam VALUEID_DEBUGGER_MEMORY_POOL = 2;

reg r_nes_reset_n;
reg [1:0] r_debugger_memory_pool;

reg [15:0] r_value;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_nes_reset_n <= 1;
    end
    else
    begin                
        if (i_ena)
        begin
            if (i_wea)
            begin
                case (i_id)
                VALUEID_NES_RESET_N: begin
                    r_nes_reset_n <= (i_data == 1);
                end
                VALUEID_DEBUGGER_MEMORY_POOL: begin
                    r_debugger_memory_pool <= i_data[1:0];
                end
                default: begin
                end
                endcase
            end
        end
    end
end

always @(*)
begin
    case (i_id)
    VALUEID_NES_RESET_N: begin
        r_value = { 15'd0, r_nes_reset_n };
    end
    VALUEID_DEBUGGER_MEMORY_POOL: begin
        r_value = { 14'd0, r_debugger_memory_pool };
    end
    default:
        r_value = 0;
    endcase
end

assign o_data = i_ena ? r_value : 0;
assign o_nes_reset_n = r_nes_reset_n;
assign o_debugger_memory_pool = r_debugger_memory_pool;

endmodule
