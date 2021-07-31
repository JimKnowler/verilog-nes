module Cpu6502(
    input i_clk,
    input i_reset_n,

    input i_clk_en,                         // clock enable

    output o_rw,                            // Read / Write - where 1 = READ, 0 = WRITE
    output [15:0] o_address,
    input [7:0] i_data,                     // 8 bit data - used for READ
    output [7:0] o_data,                    // 8 bit data - used for WRITE

    input i_irq_n,                          // Interrupt Request
    input i_nmi_n,                          // Non Maskable Interrupt

    output o_sync,                          // 1 for clock cycle where opcode is loaded

    // debug ports
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

    output o_debug_error                    // set to 1 if error occurs 
                                            // (most likely unsupported opcode)

    // TODO defines: to turn on/off sections of debugging
);

// internal busses
wire [7:0] w_bus_db;
wire [7:0] w_bus_adl;
wire [7:0] w_bus_adh;
wire [7:0] w_bus_sb;

// Module/Register outputs
wire [7:0] w_dl;
wire [7:0] w_pcl; 
wire [7:0] w_ir;
wire [7:0] w_s;
wire [7:0] w_add;
wire w_acr;
wire w_avr;
wire [7:0] w_p;
wire [7:0] w_abh;
wire [7:0] w_abl;
wire [7:0] w_pch;
wire [7:0] w_x;
wire [7:0] w_y;
wire [7:0] w_ac;

// Timing Control Unit
wire [3:0] w_tcu;
wire [3:0] w_tcu_next;
wire w_sync;
TCU tcu(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_tcu_next(w_tcu_next),
    .o_tcu(w_tcu),
    .o_sync(w_sync)
);

// instruction register
IR ir(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_data(i_data),
    .i_tcu_next(w_tcu_next),
    .o_ir(w_ir)
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
wire w_dbz_z;
wire w_db0_c;
wire w_db1_z;
wire w_db2_i;
wire w_db3_d;
wire w_db4_b;
wire w_db6_v;
wire w_db7_n;
wire w_acr_c;
wire w_ir5_c;
wire w_ir5_i;
wire w_ir5_d;
wire w_avr_v;
wire w_error;
wire w_1_db4;

// Decoder
// transform IR and TCU into control signals
Decoder decoder(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_ir(w_ir),
    .i_tcu(w_tcu),
    .i_p(w_p),
    .i_acr(w_acr),
    .i_bus_db_n(w_bus_db[7]),
    .i_irq_n(i_irq_n),
    .i_nmi_n(i_nmi_n),
    .o_tcu(w_tcu_next),
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
    .o_srs(w_srs),
    .o_dbz_z(w_dbz_z),
    .o_db0_c(w_db0_c),
    .o_db1_z(w_db1_z),
    .o_db2_i(w_db2_i),
    .o_db3_d(w_db3_d),
    .o_db4_b(w_db4_b),
    .o_db6_v(w_db6_v),
    .o_db7_n(w_db7_n),
    .o_acr_c(w_acr_c),
    .o_ir5_c(w_ir5_c),
    .o_ir5_i(w_ir5_i),
    .o_ir5_d(w_ir5_d),
    .o_avr_v(w_avr_v),
    .o_error(w_error),
    .o_1_db4(w_1_db4)
);

// Input Data Latch
DL dl(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_data(i_data),
    .o_data(w_dl)
);

// Data Output Register 
// (including data bus tristate buffers)
DOR dor(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_rw(w_rw),
    .i_data(w_bus_db),
    .o_data(o_data)
);

// Program Counter Low
PCL pcl(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_pcl_pcl(w_pcl_pcl),
    .i_adl_pcl(w_adl_pcl),
    .i_adl(w_bus_adl),
    .i_i_pc(w_i_pc),
    .o_pclc(w_pclc),
    .o_pcl(w_pcl)
);

// Program Counter High
PCH pch(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_pch_pch(w_pch_pch),
    .i_adh_pch(w_adh_pch),
    .i_adh(w_bus_adh),
    .i_pclc(w_pclc),
    .o_pch(w_pch)
);

// X Register
Register #(.DEFAULT_VALUE(0)) x
(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_data(w_bus_sb),
    .i_load(w_sb_x),
    .o_data(w_x)
);

// Y Register
Register #(.DEFAULT_VALUE(0)) y(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_data(w_bus_sb),
    .i_load(w_sb_y),
    .o_data(w_y)
);

// Accumulator Register (ac)
Register #(.DEFAULT_VALUE(0)) ac(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_data(w_bus_sb),
    .i_load(w_sb_ac),
    .o_data(w_ac)
);

// Stack Register (s)
Register  #(.DEFAULT_VALUE(0)) s(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_data(w_bus_sb),
    .i_load(w_sb_s),
    .o_data(w_s)
);

/// ALU
ALU alu(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
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
    .o_acr(w_acr),
    .o_add(w_add),
    .o_avr(w_avr)
);

// Processor Status Register (p)
ProcessorStatus p(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .o_p(w_p),
    .i_db(w_bus_db),
    .i_dbz_z(w_dbz_z),
    .i_db0_c(w_db0_c),
    .i_db1_z(w_db1_z),
    .i_db2_i(w_db2_i),
    .i_db3_d(w_db3_d),
    .i_db4_b(w_db4_b),
    .i_db6_v(w_db6_v),
    .i_db7_n(w_db7_n),
    .i_acr(w_acr),
    .i_acr_c(w_acr_c),
    .i_ir5_c(w_ir5_c),
    .i_ir5(w_ir[5]),
    .i_ir5_i(w_ir5_i),
    .i_ir5_d(w_ir5_d),
    .i_avr(w_avr),
    .i_avr_v(w_avr_v)
);

// Address bus register - High
AddressBusRegister abh(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
    .i_address(w_bus_adh),
    .i_load(w_adh_abh),
    .o_address(w_abh)
);

// Address bus register - Low
AddressBusRegister abl(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_clk_en(i_clk_en),
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
    .o_bus_adh(w_bus_adh),
    .i_1_db4(w_1_db4)
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
assign o_debug_ac = w_ac;
assign o_debug_x = w_x;
assign o_debug_y = w_y;
assign o_debug_p = w_p;
assign o_debug_error = w_error;

endmodule
