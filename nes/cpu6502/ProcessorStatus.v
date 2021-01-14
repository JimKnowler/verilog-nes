// Processor Status Register
//
// store C, Z, I, D, B, V and N flags

module ProcessorStatus(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,

    output [7:0] o_p,

    // bus input
    input [7:0] i_db,

    // general inputs
    input i_ir5,
    input i_acr,                        // ALU Carry
    /*
    input i_avr,                        // ALU overflow

    // CARRY control signals
    input i_db0_c,
    */
    input i_ir5_c,
    input i_acr_c,

    // ZERO control signals
    /*
    input i_db1_z,
    */
    input i_dbz_z,
    /*

    // INTERRUPT control signals
    input i_db2_i,
    */
    input i_ir5_i,
    /*

    // DECIMAL control signals
    input i_db3_d,
    input i_ir5_d,

    // OVERFLOW control signals
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

reg r_c;
reg r_z;
reg r_i;
reg r_n;

// C - Carry flag
always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_c <= 0;
    end
    else
    begin
        if (i_acr_c)
            r_c <= i_acr;
        else if (i_ir5_c)
            r_c <= i_ir5;
    end
end

// Z - zero flag
always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
        r_z <= 0;
    else
    begin
        if (i_dbz_z)
            r_z <= w_dbz;
    end
end

// I - Interrupt Flag
always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
        r_i <= 0;
    else
    begin
        if (i_ir5_i)
            r_i <= i_ir5;
    end 
end

// N - Negative Flag
always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
        r_n <= 0;
    else 
    begin
        if (i_db7_n)
            r_n <= i_db[N];
    end
        
end

assign o_p[C] = r_c;
assign o_p[Z] = r_z;
assign o_p[I] = r_i;
assign o_p[D] = 0;
assign o_p[B] = 0;
assign o_p[V] = 0;
assign o_p[N] = r_n;

endmodule
