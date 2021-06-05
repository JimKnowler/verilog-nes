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

Cpu6502 cpu(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .o_rw(o_rw),
    .o_address(o_address),
    .i_data(i_data),
    .o_data(o_data),
    .i_irq_n(i_irq_n),
    .i_nmi_n(i_nmi_n),
    .o_sync(o_sync),
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

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        
    end
    else
    begin
        
    end
end

endmodule
