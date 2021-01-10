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
    */
    input i_db1_z,
    input i_db7_n
    /*
    input i_db3_d,
    input i_ir5_d,

    input i_db6_v,
    input i_avr_v,
    input i_1_v,

    input i_db7_n
    */
    /* verilator lint_on UNUSED */
);

localparam C = 0;       // Carry Flag
localparam Z = 1;       // Zero Flag
localparam I = 2;       // Interrupt Disable
localparam D = 3;       // Decimal Mode Flag
localparam B = 4;       // Break Command
localparam V = 6;       // Overflow Flag
localparam N = 7;       // Negative Flag

/* verilator lint_off UNUSED */
wire [7:0] w_dbz;
/* verilator lint_on UNUSED */
assign w_dbz = ~i_db;

reg [7:0] r_p;

// N - Negative Flag
always @(negedge i_reset_n or negedge i_db7_n)
begin
    if (!i_reset_n)
        r_p[N] <= 0;
    else 
        r_p[N] <= i_db[N];
end

assign o_p = r_p;

endmodule
