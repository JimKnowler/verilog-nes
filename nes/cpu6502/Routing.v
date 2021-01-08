module Routing(
    /* verilator lint_off UNUSED */
    // todo: support combinatorial modules in gtestverilog
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    // Input Data Latch (DL)
    input [7:0] i_dl,
    input i_dl_db,
    input i_dl_adl,
    input i_dl_adh,

    // Program Counter Low (PCL)
    input [7:0] i_pcl,
    input i_pcl_adl,
    input i_pcl_db,

    // Program Counter High (PCH)
    input [7:0] i_pch,
    input i_pch_adh,
    input i_pch_db,

    // X register
    input [7:0] i_x,
    input i_x_sb,

    // Y register
    input [7:0] i_y,
    input i_y_sb,

    // Accumulator (AC)
    input [7:0] i_ac,
    input i_ac_sb,
    input i_ac_db,

    // Stack Pointer (S)
    input [7:0] i_s,
    input i_s_sb,
    input i_s_adl,

    // Adder Hold Register (ADD)
    input [7:0] i_add,
    input i_add_sb_7,
    input i_add_sb_0_6,
    input i_add_adl,

    // Processor Status Register (P)
    input [7:0] i_p,
    input i_p_db,

    // Open Drain Mosfets
    input i_0_adl0,
    input i_0_adl1,
    input i_0_adl2,
    input i_0_adh0,
    input i_0_adh1_7,

    // Pass Mosfets
    input i_sb_adh,
    input i_sb_db,

    // output bus values
    output [7:0] o_bus_db,
    output [7:0] o_bus_sb,
    output [7:0] o_bus_adl,
    output [7:0] o_bus_adh
);

reg [7:0] r_bus_db;
reg [7:0] r_bus_sb;
reg [7:0] r_bus_adl;
reg [7:0] r_bus_adh;

// Drive DB bus
always @(*) 
begin
    r_bus_db = 8'hFF;

    if (i_dl_db)
        r_bus_db = i_dl;
    else if (i_pcl_db) 
        r_bus_db = i_pcl;
    else if (i_pch_db)
        r_bus_db = i_pch;
    else if (i_ac_db)
        r_bus_db = i_ac;
    else if (i_p_db)
        r_bus_db = i_p;
    else if (i_sb_db)
        r_bus_db = r_bus_sb;
end

// Drive SB bus
always @(*)
begin
    r_bus_sb = 8'hFF;

    if (i_x_sb)
        r_bus_sb = i_x;
    else if (i_y_sb)
        r_bus_sb = i_y;
    else if (i_ac_sb)
        r_bus_sb = i_ac;
    else if (i_s_sb)
        r_bus_sb = i_s;
    else if (i_add_sb_7)
        r_bus_sb[7] = i_add[7];
    else if (i_add_sb_0_6)
        r_bus_sb[6:0] = i_add[6:0];
    else if (i_dl_db && i_sb_db)
        r_bus_sb = i_dl;
end

// Drive ADL bus
always @(*)
begin
    r_bus_adl = 8'hFF;

    if (i_dl_adl)
        r_bus_adl = i_dl;
    if (i_pcl_adl)
        r_bus_adl = i_pcl;
    if (i_s_adl)
        r_bus_adl = i_s;
    if (i_add_adl)
        r_bus_adl = i_add;

    // open drain mosfets
    if (i_0_adl0)
        r_bus_adl[0] = 0;
    if (i_0_adl1)
        r_bus_adl[1] = 0;
    if (i_0_adl2)
        r_bus_adl[2] = 0;
end

// Drive ADH bus
always @(*)
begin
    r_bus_adh = 8'hFF;

    if (i_dl_adh)
        r_bus_adh = i_dl;
    if (i_pch_adh)
        r_bus_adh = i_pch;
    if (i_sb_adh)
        r_bus_adh = r_bus_sb;

    // open drain mosfets
    if (i_0_adh0)
        r_bus_adh[0] = 0;
    if (i_0_adh1_7)
        r_bus_adh[7:1] = 0;
end

assign o_bus_db = r_bus_db;
assign o_bus_sb = r_bus_sb;
assign o_bus_adl = r_bus_adl;
assign o_bus_adh = r_bus_adh;

endmodule