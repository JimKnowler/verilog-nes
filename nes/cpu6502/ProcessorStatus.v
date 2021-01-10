// Processor Status Register
//
// store C, Z, I, D, B, V and N flags

module ProcessorStatus(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,

    output [7:0] o_p,

    input [7:0] i_db,

    /*
    input i_ir5,
    input i_acr,                        // ALU Carry
    input i_avr,                        // ALU overflow

    input i_db0_c,
    input i_ir5_c,
    input i_acr_c,

    input i_db1_z,
    */
    input i_dbz_z,
    /*

    input i_db3_d,
    input i_ir5_d,

    input i_db6_v,
    input i_avr_v,
    input i_1_v,
    */

    input i_db7_n
    /* verilator lint_on UNUSED */
);

localparam C = 0;       // Carry Flag
localparam Z = 1;       // Zero Flag
localparam I = 2;       // Interrupt Disable
localparam D = 3;       // Decimal Mode Flag
localparam B = 4;       // Break Command
localparam V = 6;       // Overflow Flag
localparam N = 7;       // Negative Flag

wire w_dbz;
assign w_dbz = ~(|i_db);

reg r_z;
reg r_n;

// Z - zero flag
always @(negedge i_reset_n or negedge i_dbz_z)
begin
    if (!i_reset_n)
        r_z <= 0;
    else
        r_z <= w_dbz;
end

// N - Negative Flag
always @(negedge i_reset_n or negedge i_db7_n)
begin
    if (!i_reset_n)
        r_n <= 0;
    else 
        r_n <= i_db[N];
end


assign o_p[C] = 0;
assign o_p[Z] = r_z;
assign o_p[I] = 0;
assign o_p[D] = 0;
assign o_p[B] = 0;
assign o_p[V] = 0;
assign o_p[N] = r_n;

endmodule
