// Arithmetic Logic Unit
// Including:
// - A Input Register
// - B Input Register
// - Arithmetic Logic
// - Adder Hold Register (ADD)
//
// o_add: Implemented as latch on rising edge of phi2
// o_avr/o_acr: Implemented as latch on rising edge of phi2
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
    output o_avr,           // overflow signal
    output o_acr,           // carry signal

    // Adder Hold Register
    output [7:0] o_add
);

reg [7:0] r_a;
reg [7:0] r_b;
reg [7:0] r_alu;
reg [7:0] r_add;
reg r_acr;
reg r_avr;

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

// generate SUM logic with Full Adder
wire [7:0] w_alu_sum;
genvar i;
/* verilator lint_off UNOPTFLAT */
wire [8:0] w_carry;
/* verilator lint_on UNOPTFLAT */
assign w_carry[0] = i_1_addc;
generate
    for (i=0; i<8; i=i+1) begin
        ALUFullAdder fullAdder(
            .i_a(r_a[i]),
            .i_b(r_b[i]),
            .i_carry_in(w_carry[i]),
            .o_sum(w_alu_sum[i]),
            .o_carry_out(w_carry[i+1])
        );
    end
endgenerate

// ALU calculation
always @(*)
begin
    // default values
    r_alu = 8'h00;
    r_acr = 0;
    r_avr = 0;

    if (i_sums)
    begin
        r_alu = w_alu_sum[7:0];
        r_acr = w_carry[8];
        r_avr = w_carry[7] ^ w_carry[8];
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
        r_alu[7] = i_1_addc;
        r_acr = r_a[0];
    end
end

// Adder Hold Register (ADD)
always @(posedge i_clk or negedge i_reset_n) begin
    if (!i_reset_n)
        r_add <= 0;
    else
        r_add <= r_alu;
end

// Carry / Overflow outputs
reg r_acr_out;
reg r_avr_out;

always @(posedge i_clk or negedge i_reset_n) begin
    if (!i_reset_n)
    begin
        r_acr_out <= 0;
        r_avr_out <= 0;
    end
    else
    begin
        r_acr_out <= r_acr;
        r_avr_out <= r_avr;
    end
end

assign o_add = r_add;
assign o_acr = r_acr_out;
assign o_avr = r_avr_out;

endmodule
