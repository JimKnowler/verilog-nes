//
// 2A03 - NES Cpu that contains 6502 + OAM DMA + APU
//

module Cpu2A03(
    input i_clk,
    input i_reset_n,
    
    // 6502 ports
    input i_clk_en,
    output o_rw,
    output [15:0] o_address,
    input [7:0] i_data,
    output [7:0] o_data,
    input i_irq_n,
    input i_nmi_n,
    output o_sync,

    // controller ports
    output o_out0,              // strobe signal for both controller ports
    output o_oe1_n,             // enable output of controller 1

    // 6502 debug ports
    output [7:0] o_debug_bus_db,
    output [7:0] o_debug_bus_adl,
    output [7:0] o_debug_bus_adh,
    output [7:0] o_debug_bus_sb,
    output [7:0] o_debug_ir,
    output [3:0] o_debug_tcu,
    output [7:0] o_debug_s,
    output [7:0] o_debug_pcl,
    output [7:0] o_debug_pch,
    output [7:0] o_debug_add,
    output [7:0] o_debug_dl,
    output [7:0] o_debug_ac,
    output [7:0] o_debug_x,
    output [7:0] o_debug_y,
    output [7:0] o_debug_p,
    output o_debug_error
);

localparam [15:0] ADDRESS_OAMDMA = 16'h4014; 
localparam [15:0] ADDRESS_OAMDATA = 16'h2004;
localparam [15:0] ADDRESS_JOY1 = 16'h4016;

localparam RW_WRITE = 0;
localparam RW_READ = 1;

reg [1:0] r_state;
localparam [1:0] STATE_CPU = 0;
localparam [1:0] STATE_OAMDMA_START = 1;
localparam [1:0] STATE_OAMDMA_READ = 2;
localparam [1:0] STATE_OAMDMA_WRITE = 3;

wire [15:0] w_address_cpu;
wire [7:0] w_o_data_cpu;
reg [7:0] r_o_data;

reg r_clk_en_cpu;

reg r_out0;
reg r_oe1_n;

always @(*)
begin
    case (r_state)
    STATE_CPU:
        r_clk_en_cpu = i_clk_en;
    default:
        r_clk_en_cpu = 0;
    endcase
end

always @(*)
begin
    case (r_state)
    STATE_CPU:
        r_o_data = w_o_data_cpu;
    STATE_OAMDMA_START, STATE_OAMDMA_READ:
        r_o_data = 0;
    STATE_OAMDMA_WRITE:
        r_o_data = r_oamdma_data;
    endcase
end

reg r_rw;
reg r_rw_cpu;

always @(*)
begin
    case (r_state)
    STATE_CPU:
        r_rw = r_rw_cpu;
    STATE_OAMDMA_START:
        r_rw = RW_READ;
    STATE_OAMDMA_READ:
        r_rw = RW_READ;
    STATE_OAMDMA_WRITE:
        r_rw = RW_WRITE;
    endcase
end

reg r_sync;
reg r_sync_cpu;

always @(*)
begin
    case (r_state)
    STATE_CPU:
        r_sync = r_sync_cpu;
    default:
        r_sync = 0;
    endcase
end

Cpu6502 cpu6502(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(r_clk_en_cpu),
    .o_rw(r_rw_cpu),
    .o_address(w_address_cpu),
    .i_data(i_data),
    .o_data(w_o_data_cpu),
    .i_irq_n(i_irq_n),
    .i_nmi_n(i_nmi_n),
    .o_sync(r_sync_cpu),
    .o_debug_bus_db(o_debug_bus_db),
    .o_debug_bus_adl(o_debug_bus_adl),
    .o_debug_bus_adh(o_debug_bus_adh),
    .o_debug_bus_sb(o_debug_bus_sb),
    .o_debug_ir(o_debug_ir),
    .o_debug_tcu(o_debug_tcu),
    .o_debug_s(o_debug_s),
    .o_debug_pcl(o_debug_pcl),
    .o_debug_pch(o_debug_pch),
    .o_debug_add(o_debug_add),
    .o_debug_dl(o_debug_dl),
    .o_debug_ac(o_debug_ac),
    .o_debug_x(o_debug_x),
    .o_debug_y(o_debug_y),
    .o_debug_p(o_debug_p),
    .o_debug_error(o_debug_error)
);

reg [7:0] r_oamdma_counter;
reg [15:0] r_oamdma_address;
reg [7:0] r_oamdma_data;

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_state <= STATE_CPU;
    end
    else if (i_clk_en)
    begin
        case (r_state)
        STATE_CPU: begin
            if (w_address_cpu == ADDRESS_OAMDMA)
            begin
                r_oamdma_address[15:8] <= w_o_data_cpu;
                r_oamdma_address[7:0] <= 0;
                r_state <= STATE_OAMDMA_START;
            end
        end
        STATE_OAMDMA_START: begin
            r_oamdma_counter <= 255;
            r_state <= STATE_OAMDMA_READ;
        end
        STATE_OAMDMA_READ: begin
            r_state <= STATE_OAMDMA_WRITE;
            r_oamdma_data <= i_data;
        end
        STATE_OAMDMA_WRITE: begin
            r_oamdma_counter <= r_oamdma_counter - 1;
            r_oamdma_address <= r_oamdma_address + 1;

            if (r_oamdma_counter == 0)
                r_state <= STATE_CPU;
            else
                r_state <= STATE_OAMDMA_READ;
            
        end
        default: begin 
        end
        endcase
    end
end

reg [15:0] r_address;

always @(*)
begin
    case (r_state)
    STATE_OAMDMA_READ: begin
        r_address = r_oamdma_address;
    end
    STATE_OAMDMA_WRITE: begin
        r_address = ADDRESS_OAMDATA;
    end
    STATE_OAMDMA_START: begin
        r_address = 0;
    end
    default: begin
        r_address = w_address_cpu;
    end
    endcase
end

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_out0 <= 0;
    end
    else if (i_clk_en)
    begin
        if (r_rw_cpu == RW_WRITE)
        begin
            if (w_address_cpu == ADDRESS_JOY1)
            begin
                r_out0 <= w_o_data_cpu[0];
            end
        end
    end
end

always @(*)
begin
    r_oe1_n = 1;

    if (i_clk_en && (r_rw_cpu == RW_READ) && (w_address_cpu == ADDRESS_JOY1))
    begin
        r_oe1_n = 0;
    end
end

assign o_address = r_address;
assign o_data = r_o_data;
assign o_rw = r_rw;
assign o_sync = r_sync;

assign o_out0 = r_out0;
assign o_oe1_n = r_oe1_n;

endmodule
