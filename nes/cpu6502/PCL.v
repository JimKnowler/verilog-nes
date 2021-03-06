// Program Counter Low
//
// combination of components for Program Counter low byte
// - Program Counter Low Select (PCLS)
// - Increment logic
// - Program Counter Low Register (PCL)
//
// Implemented as Latch on falling edge of phi2

module PCL(
    input i_clk,
    input i_reset_n,

    input i_clk_en,             // clock enable

    // program counter low select register
    input i_pcl_pcl,        // Control Signal - input from PCL
    input i_adl_pcl,        // Control Signal - input from ADL
    input [7:0] i_adl,      // ADL bus

    // increment logic
    input i_i_pc,           // increment
    output reg o_pclc,      // carry out

    // program counter low register
    output [7:0] o_pcl
);

reg [8:0] r_pcls;       // output of PCLS
reg [8:0] r_pcls_inc;   // output of increment logic
                        // NOTE: 8th bit => carry out
reg [7:0] r_pcl;        // output of PCL

// PCLS
always @(i_pcl_pcl or i_adl_pcl or i_adl or r_pcl)
begin
    r_pcls[8] = 0;

    if (i_pcl_pcl)
        r_pcls[7:0] = r_pcl;
    else if (i_adl_pcl)
        r_pcls[7:0] = i_adl;
end

// Increment Logic
always @(r_pcls or i_i_pc)
begin
    r_pcls_inc = r_pcls + { 8'b0, i_i_pc };
end

// Carry Out
always @(r_pcls_inc)
begin
    o_pclc = r_pcls_inc[8];
end

wire [7:0] w_pcls_inc_output = r_pcls_inc[7:0];

// Program Counter Low Register
always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
        r_pcl <= 0;
    else if (i_clk_en)
        r_pcl <= w_pcls_inc_output;
end

assign o_pcl = r_pcl;

endmodule