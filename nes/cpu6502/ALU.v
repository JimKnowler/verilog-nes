// Arithmetic Logic Unit
// Including:
// - A Input Register
// - B Input Register
// - Arithmetic Logic
// - Adder Hold Register (ADD)
//
// o_add: Implemented as latch on falling edge of phi2
// o_avr/o_acr: Implemented as combinatorial logic
//
// note: not implemented
// - i_daa: decimal enable
// - o_hc: half carry (used by decimal adj adders)
//
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
    input i_1_addc,         // carry in
    input i_sums,           // a + b
    input i_ands,           // a & b
    input i_eors,           // a ^ b
    input i_ors,            // a | b
    input i_srs,            // a >> 1
    //output o_avr,           // overflow signal
    output o_acr,           // carry signal

    // Adder Hold Register
    output [7:0] o_add
);

reg [7:0] r_a;
reg [7:0] r_b;
reg [7:0] r_alu;
reg [7:0] r_add;
reg r_acr;

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
    r_acr = 0;

    if (i_sums)
    begin
        r_alu = r_a + r_b + ( i_1_addc ? 8'h01 : 8'h00);
        r_acr = r_a[7] & r_b[7];
    end
    else if (i_ands)
        r_alu = r_a & r_b;
    else if (i_eors)
        r_alu = r_a ^ r_b;
    else if (i_ors)
        r_alu = r_a | r_b;
    else if (i_srs)
    begin
        r_alu = r_a >> 1;
        r_acr = r_a[0];
    end
end

// Adder Hold Register (ADD)
always @(negedge i_clk or negedge i_reset_n) begin
    if (!i_reset_n)
        r_add <= 0;
    else
        r_add <= r_alu;
end

assign o_add = r_add;
assign o_acr = r_acr;

endmodule
