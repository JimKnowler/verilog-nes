// PPU - Picture Processing Unit
// AKA 2C02

module PPU(
    /* verilator lint_off UNUSED */
    /* verilator lint_off UNDRIVEN */
    input i_clk,
    input i_reset_n,

    // CPU interface
    output o_int_n,                 // ~Interrupt, to drive ~NMI on CPU
    input [2:0] i_rs,               // register select
    input [7:0] i_data,             // Read from CPU data bus
    output [7:0] o_data,            // Write to CPU data bus
    input i_rw,                     // Read/~Write for CPU data bus

    // VIDEO interface    
    output o_rd_n,                  // ~Read from VIDEO data bus
    output o_we_n,                  // ~Write to VIDEO data bus
    output [13:0] o_vaddress,       // address for 
    output [7:0] o_vdata,
    input [7:0] i_vdata,

    // VGA output
    output [7:0] o_vga_red,
    output [7:0] o_vga_green,
    output [7:0] o_vga_blue,
    output o_vga_h_sync,
    output o_vga_v_sync

    /* verilator lint_on UNDRIVEN */
    /* verilator lint_on UNUSED */

);

reg r_int_n;
reg r_rd_n;
reg r_we_n;

always @(*)
begin
    r_int_n = 1;
    r_rd_n = 1;
    r_we_n = 1;
end

assign o_int_n = r_int_n;
assign o_rd_n = r_rd_n;
assign o_we_n = r_we_n;

endmodule