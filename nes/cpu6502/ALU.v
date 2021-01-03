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
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,

    // B Input Register
    input i_db_n_add,       // ~db
    input i_db_add,         // db
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
    output [7:0] o_add      // ADD register
    /* verilator lint_on UNDRIVEN */
);

endmodule
