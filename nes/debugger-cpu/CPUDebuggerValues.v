/*
 * Write: VALUEID_CPU_STEP = 1, to trigger a single step of CPU
 * Read: VALUEID_CPU_STEP, ==1 while stepping, ==0 when finished stepping
 * Read: VALUDID_CPU_xxxx to read state of CPU
 */

module CPUDebuggerValues(
    input i_clk,
    input i_reset_n,

    input i_ena,
    input i_wea,
    input [15:0] i_id,
    input [15:0] i_data,
    output [15:0] o_data,

    // CPU fields
    input [15:0] i_cpu_address,
    input [7:0] i_cpu_data,
    input i_cpu_rw,
    input i_cpu_irq_n,
    input i_cpu_nmi_n,
    input i_cpu_sync,
    input [7:0] i_cpu_reg_a,
    input [7:0] i_cpu_reg_x,
    input [7:0] i_cpu_reg_y,
    input [7:0] i_cpu_reg_s,
    input [7:0] i_cpu_reg_p,
    input [7:0] i_cpu_reg_ir,
    

    // CPU step control signals
    output o_cpu_step,
    input i_cpu_step_completed,

    // CPU reset_n signal
    output o_cpu_reset_n
);

// Single Step CPU clock cycle by writing 1
//  - step is complete when this value reads as 0
localparam VALUEID_CPU_STEP = 1;

// Set the value of RESET_N pin on the CPU 
localparam VALUEID_CPU_RESET_N = 14;

// Retrieve CPU values
localparam VALUEID_CPU_ADDRESS = 2;
localparam VALUEID_CPU_DATA = 3;
localparam VALUEID_CPU_RW = 4;
localparam VALUEID_CPU_IRQ_N = 5;
localparam VALUEID_CPU_NMI_N = 6;
localparam VALUEID_CPU_SYNC = 7;
localparam VALUEID_CPU_REG_A = 8;
localparam VALUEID_CPU_REG_X = 9;
localparam VALUEID_CPU_REG_Y = 10;
localparam VALUEID_CPU_REG_S = 11;
localparam VALUEID_CPU_REG_P = 12;
localparam VALUEID_CPU_REG_IR = 13;

reg r_cpu_step;
reg r_cpu_reset_n;

reg [15:0] r_value;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_cpu_step <= 0;
        r_cpu_reset_n <= 1;
    end
    else
    begin
        if (i_cpu_step_completed == 1)
        begin
            r_cpu_step <= 0;
        end
                
        if (i_ena)
        begin
            if (i_wea)
            begin
                case (i_id)
                VALUEID_CPU_STEP: begin
                    r_cpu_step <= (i_data == 1);
                end
                VALUEID_CPU_RESET_N: begin
                    r_cpu_reset_n <= (i_data == 1);
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
    VALUEID_CPU_STEP: begin
        r_value = { 15'd0, r_cpu_step };
    end
    VALUEID_CPU_RESET_N: begin
        r_value = { 15'd0, r_cpu_reset_n };
    end
    VALUEID_CPU_ADDRESS: begin
        r_value = i_cpu_address;
    end
    VALUEID_CPU_DATA: begin
        r_value = { 8'd0, i_cpu_data };
    end
    VALUEID_CPU_RW: begin
        r_value = { 15'd0, i_cpu_rw };
    end
    VALUEID_CPU_IRQ_N: begin
        r_value = { 15'd0, i_cpu_irq_n };
    end
    VALUEID_CPU_NMI_N: begin
        r_value = { 15'd0, i_cpu_nmi_n };
    end
    VALUEID_CPU_SYNC: begin
        r_value = { 15'd0, i_cpu_sync };
    end
    VALUEID_CPU_REG_A: begin
        r_value = { 8'd0, i_cpu_reg_a };
    end
    VALUEID_CPU_REG_X: begin
        r_value = { 8'd0, i_cpu_reg_x };
    end
    VALUEID_CPU_REG_Y: begin
        r_value = { 8'd0, i_cpu_reg_y };
    end
    VALUEID_CPU_REG_S: begin
        r_value = { 8'd0, i_cpu_reg_s };
    end
    VALUEID_CPU_REG_P: begin
        r_value = { 8'd0, i_cpu_reg_p };
    end
    VALUEID_CPU_REG_IR: begin
        r_value = { 8'd0, i_cpu_reg_ir };
    end
    default:
        r_value = 0;
    endcase
end

assign o_data = i_ena ? r_value : 0;
assign o_cpu_step = r_cpu_step;
assign o_cpu_reset_n = r_cpu_reset_n;

endmodule