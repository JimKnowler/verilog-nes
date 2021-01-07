module Cpu6502(
    input i_clk,
    input i_reset_n,

    output o_rw,                            // Read / Write - where 1 = READ, 0 = WRITE
    output [15:0] o_address,
    input [7:0] i_data,                     // 8 bit data - used for READ
    output [7:0] o_data,                    // 8 bit data - used for WRITE

    output o_sync,                          // 1 for clock cycle where opcode is loaded

    // debug ports
    output [7:0] o_debug_bus_db,
    output [7:0] o_debug_bus_adl,
    output [7:0] o_debug_bus_adh,
    output [7:0] o_debug_bus_sb,
    output [7:0] o_debug_ir,
    output [2:0] o_debug_tcu,
    output [7:0] o_debug_s,
    output [7:0] o_debug_pcl,
    output [7:0] o_debug_pch,
    output [7:0] o_debug_add,
    output [7:0] o_debug_dl

    // TODO defines: to turn on/off sections of debugging
);

// internal busses
wire [7:0] w_bus_db;
wire [7:0] w_bus_adl;
wire [7:0] w_bus_adh;
wire [7:0] w_bus_sb;

// Timing Control Unit
wire [2:0] w_tcu;
wire [2:0] w_tcu_next;
wire w_sync;
TCU tcu(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_tcu_next(w_tcu_next),
    .o_tcu(w_tcu),
    .o_sync(w_sync)
);

// instruction register
wire [7:0] w_ir;
wire w_interrupt;
IR ir(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_data(i_data),
    .i_tcu(w_tcu),
    .o_ir(w_ir),
    .i_interrupt(w_interrupt)
);

// control signals from Decoder
wire w_rw;
wire w_dl_db;
wire w_dl_adl;
wire w_dl_adh;
wire w_pcl_pcl;
wire w_adl_pcl;
wire w_i_pc;
wire w_pclc;
wire w_pcl_adl;
wire w_pcl_db;
wire w_pch_pch;
wire w_adh_pch;
wire w_pch_adh;
wire w_pch_db;
wire w_x_sb;
wire w_y_sb;
wire w_ac_sb;
wire w_ac_db;
wire w_s_sb;
wire w_s_adl;
wire w_add_sb_7;
wire w_add_sb_0_6;
wire w_add_adl;
wire w_p_db;
wire w_0_adl0;
wire w_0_adl1;
wire w_0_adl2;
wire w_0_adh0;
wire w_0_adh1_7;
wire w_sb_adh;
wire w_sb_db;
wire w_sb_x;
wire w_sb_y;
wire w_sb_ac;
wire w_sb_s;
wire w_adl_abl;
wire w_adh_abh;
wire w_db_n_add;
wire w_db_add;
wire w_adl_add;
wire w_0_add;
wire w_sb_add;
wire w_1_addc;
wire w_sums;
wire w_ands;
wire w_eors;
wire w_ors;
wire w_srs;

// Decoder
// transform IR and TCU into control signals
Decoder decoder(
    .i_clk,
    .i_ir(w_ir),
    .i_tcu(w_tcu),
    .o_tcu(w_tcu_next),
    .o_interrupt(w_interrupt),
    .o_rw(w_rw),
    .o_dl_db(w_dl_db),
    .o_dl_adl(w_dl_adl),
    .o_dl_adh(w_dl_adh),
    .o_pcl_pcl(w_pcl_pcl),
    .o_adl_pcl(w_adl_pcl),
    .o_i_pc(w_i_pc),
    .o_pclc(w_pclc),
    .o_pcl_adl(w_pcl_adl),
    .o_pcl_db(w_pcl_db),
    .o_pch_pch(w_pch_pch),
    .o_adh_pch(w_adh_pch),
    .o_pch_adh(w_pch_adh),
    .o_pch_db(w_pch_db),
    .o_x_sb(w_x_sb),
    .o_y_sb(w_y_sb),
    .o_ac_sb(w_ac_sb),
    .o_ac_db(w_ac_db),
    .o_s_sb(w_s_sb),
    .o_s_adl(w_s_adl),
    .o_add_sb_7(w_add_sb_7),
    .o_add_sb_0_6(w_add_sb_0_6),
    .o_add_adl(w_add_adl),
    .o_p_db(w_p_db),
    .o_0_adl0(w_0_adl0),
    .o_0_adl1(w_0_adl1),
    .o_0_adl2(w_0_adl2),
    .o_0_adh0(w_0_adh0),
    .o_0_adh1_7(w_0_adh1_7),
    .o_sb_adh(w_sb_adh),
    .o_sb_db(w_sb_db),
    .o_sb_x(w_sb_x),
    .o_sb_y(w_sb_y),
    .o_sb_ac(w_sb_ac),
    .o_sb_s(w_sb_s),
    .o_adl_abl(w_adl_abl),
    .o_adh_abh(w_adh_abh),
    .o_db_n_add(w_db_n_add),
    .o_db_add(w_db_add),
    .o_adl_add(w_adl_add),
    .o_0_add(w_0_add),
    .o_sb_add(w_sb_add),
    .o_1_addc(w_1_addc),
    .o_sums(w_sums),
    .o_ands(w_ands),
    .o_eors(w_eors),
    .o_ors(w_ors),
    .o_srs(w_srs)
);

// Input Data Latch
wire [7:0] w_dl;
DL dl(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_data(i_data),
    .o_data(w_dl)
);

// Data Output Register 
// (including data bus tristate buffers)
DOR dor(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_rw(w_rw),
    .i_data(w_bus_db),
    .o_data(o_data)
);

// Program Counter Low
wire [7:0] w_pcl;
PCL pcl(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_pcl_pcl(w_pcl_pcl),
    .i_adl_pcl(w_adl_pcl),
    .i_adl(w_bus_adl),
    .i_i_pc(w_i_pc),
    .o_pclc(w_pclc),
    .o_pcl(w_pcl)
);

// Program Counter High
wire [7:0] w_pch;
PCH pch(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_pch_pch(w_pch_pch),
    .i_adh_pch(w_adh_pch),
    .i_adh(w_bus_adh),
    .i_pclc(w_pclc),
    .o_pch(w_pch)
);

// X Register
wire [7:0] w_x;
Register x(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_data(w_bus_sb),
    .i_load(w_sb_x),
    .o_data(w_x)
);

// Y Register
wire [7:0] w_y;
Register y(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_data(w_bus_sb),
    .i_load(w_sb_y),
    .o_data(w_y)
);

// Accumulator Register (ac)
wire [7:0] w_ac;
Register ac(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_data(w_bus_sb),
    .i_load(w_sb_ac),
    .o_data(w_ac)
);

// Stack Register (s)
wire [7:0] w_s;
Register s(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_data(w_bus_sb),
    .i_load(w_sb_s),
    .o_data(w_s)
);

/// ALU
wire [7:0] w_add;
ALU alu(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_db(w_bus_db),
    .i_db_n_add(w_db_n_add),
    .i_db_add(w_db_add),
    .i_adl(w_bus_adl),
    .i_adl_add(w_adl_add),
    .i_0_add(w_0_add),
    .i_sb(w_bus_sb),
    .i_sb_add(w_sb_add),
    .i_1_addc(w_1_addc),
    .i_sums(w_sums),
    .i_ands(w_ands),
    .i_eors(w_eors),
    .i_ors(w_ors),
    .i_srs(w_srs),
    .o_add(w_add)
);

/// @todo Processor Status Register (p)
wire [7:0] w_p;
assign w_p = 8'h0;

// Address bus register - High
wire [7:0] w_abh;
AddressBusRegister abh(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_address(w_bus_adh),
    .i_load(w_adh_abh),
    .o_address(w_abh)
);

// Address bus register - Low
wire [7:0] w_abl;
AddressBusRegister abl(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_address(w_bus_adl),
    .i_load(w_adl_abl),
    .o_address(w_abl)
);

// Routing internal data buses
Routing routing(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_dl(w_dl),
    .i_dl_db(w_dl_db),
    .i_dl_adl(w_dl_adl),
    .i_dl_adh(w_dl_adh),
    .i_pcl(w_pcl),
    .i_pcl_adl(w_pcl_adl),
    .i_pcl_db(w_pcl_db),
    .i_pch(w_pch),
    .i_pch_adh(w_pch_adh),
    .i_pch_db(w_pch_db),
    .i_x(w_x),
    .i_x_sb(w_x_sb),
    .i_y(w_y),
    .i_y_sb(w_y_sb),
    .i_ac(w_ac),
    .i_ac_sb(w_ac_sb),
    .i_ac_db(w_ac_db),
    .i_s(w_s),
    .i_s_sb(w_s_sb),
    .i_s_adl(w_s_adl),
    .i_add(w_add),
    .i_add_sb_7(w_add_sb_7),
    .i_add_sb_0_6(w_add_sb_0_6),
    .i_add_adl(w_add_adl),
    .i_p(w_p),
    .i_p_db(w_p_db),
    .i_0_adl0(w_0_adl0),
    .i_0_adl1(w_0_adl1),
    .i_0_adl2(w_0_adl2),
    .i_0_adh0(w_0_adh0),
    .i_0_adh1_7(w_0_adh1_7),
    .i_sb_adh(w_sb_adh),
    .i_sb_db(w_sb_db),
    .o_bus_db(w_bus_db),
    .o_bus_sb(w_bus_sb),
    .o_bus_adl(w_bus_adl),
    .o_bus_adh(w_bus_adh)
);

// drive debug signals
assign o_debug_bus_db = w_bus_db;
assign o_debug_bus_adl = w_bus_adl;
assign o_debug_bus_adh = w_bus_adh;
assign o_debug_bus_sb = w_bus_sb;
assign o_debug_ir = w_ir;
assign o_debug_tcu = w_tcu;
assign o_address[7:0] = w_abl;
assign o_address[15:8] = w_abh;
assign o_rw = w_rw;
assign o_sync = w_sync;
assign o_debug_s = w_s;
assign o_debug_pcl = w_pcl;
assign o_debug_pch = w_pch;
assign o_debug_add = w_add;
assign o_debug_dl = w_dl;

endmodule
