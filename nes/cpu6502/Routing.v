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
    // todo:

    // Open Drain Mosfets
    input i_0_adl0,
    input i_0_adl1,
    input i_0_adl2,
    input i_0_adh0,
    input i_0_adh1_7,


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
    if (i_pcl_db) 
        r_bus_db = i_pcl;
end

// Drive SB bus
always @(*)
begin
    r_bus_sb = 8'hFF;
end

// Drive ADL bus
always @(*)
begin
    r_bus_adl = 8'hFF;

    if (i_dl_adl)
        r_bus_adl = i_dl;
    if (i_pcl_adl)
        r_bus_adl = i_pcl;
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