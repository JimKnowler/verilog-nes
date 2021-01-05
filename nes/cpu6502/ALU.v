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
    input [7:0] i_adl,
    input i_adl_add,

    // A Input Register
    input i_0_add,          // load 0
    input [7:0] i_sb,
    input i_sb_add,

    // Arithmetic Logic
    /* verilator lint_off UNUSED */
    input i_1_addc,         // carry in
    /* verilator lint_on UNUSED */
    input i_sums,           // a + b
    input i_ands,           // a & b
    input i_eors,           // a ^ b
    input i_ors,            // a | b
    input i_srs,            // a >> 1
    
    //output o_avr,           // overflow signal
    //output o_acr,           // carry signal

    output [7:0] o_add      // ADD register
);

reg [7:0] r_a;
reg [7:0] r_b;

/* verilator lint_off UNOPTFLAT */
reg [7:0] r_alu;
/* verilator lint_on UNOPTFLAT */
reg [7:0] r_add;

// B Input Register
always @(*)
begin
    // default values
    r_b = 8'hff;

    if (i_db_add)
        r_b = i_db;
    else if (i_db_n_add)
        r_b = ~i_db;
    else if (i_adl_add)
        r_b = i_adl;
end

// A Input Register
always @(*)
begin
    // default value
    r_a = 8'hff;

    if (i_0_add)
        r_a = 0;
    else if (i_sb_add)
        r_a = i_sb;
end

// ALU calculation
always @(*)
begin
    // default values
    r_alu = 8'h00;

    if (i_sums)
        r_alu = r_a + r_b;
    else if (i_ands)
        r_alu = r_a & r_b;
    else if (i_eors)
        r_alu = r_a ^ r_b;
    else if (i_ors)
        r_alu = r_a | r_b;
    else if (i_srs)
        r_alu = r_b >> 1;
end

// Adder Hold Register (ADD)
always @(negedge i_clk or negedge i_reset_n) begin
    if (!i_reset_n)
        r_add <= 0;
    else
        r_add <= r_alu;
end

assign o_add = (i_clk) ? r_alu : r_add;

endmodule
