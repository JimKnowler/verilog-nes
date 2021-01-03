// Arithmetic Logic Unit
// Including:
// - A Input Register
// - B Input Register
// - Arithmetic Logic
// - Adder Hold Register (ADD)
// 
// note: not implemented
// - i_daa: decimal enable
// - o_hc: half carry (used by decimal adj adders)
// - 
module ALU( 
    input i_clk,
    input i_reset_n,

    // B Input Register
    input [7:0] i_db,
    input i_db_n_add,       // invert db
    input i_db_add,
    /* verilator lint_off UNUSED */
    input [7:0] i_bus_adl,
    input i_adl_add,

    // A Input Register
    input i_0_add,          // load 0
    input i_sb_add,

    // Arithmetic Logic
    input i_1_addc,         // carry in
    input i_sums,           // a + b
    input i_ands,           // a & b
    input i_eors,           // a ^ b
    input i_ors,            // a | b
    input i_srs,            // a >> 1
    
    /* verilator lint_on UNUSED */

    /* verilator lint_off UNDRIVEN */
    output o_avr,           // overflow signal
    output o_acr,           // carry signal
    /* verilator lint_on UNDRIVEN */

    output [7:0] o_add      // ADD register
);

reg [7:0] w_add;
reg [7:0] r_add;

always @(*)
begin
    w_add = 0;

    if (i_db_add)
        w_add = i_db;
    else if (i_db_n_add)
        w_add = ~i_db;
end

always @(posedge i_clk or negedge i_reset_n) begin
    if (!i_reset_n)
        r_add <= 0;
    else
        r_add <= w_add;
end

assign o_add = r_add;

endmodule
