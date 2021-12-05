// Program Counter High
//
// combination of components for Program Counter high byte
// - Program Counter High Select (PCHS)
// - Increment logic
// - Program Counter High Register (PCH)
//
// Implemented as Latch on falling edge of phi2

module PCH(
    input i_clk,
    input i_reset_n,

    input i_clk_en,             // clock enable

    // program counter high select register
    input i_pch_pch,        // control signal - input from PCH
    input i_adh_pch,        // control signal - input from ADH
    input [7:0] i_adh,
    
    // increment logic
    input i_pclc,           // control signal - carry in from PCL

    // program counter high register
    output [7:0] o_pch
);

reg [7:0] r_pchs;           // output of PCHS
reg [7:0] r_pchs_inc;       // output of increment logic
reg [7:0] r_pch;            // output of PCH

// PCHS
always @(*)
begin
    if (i_pch_pch)
        r_pchs = r_pch;
    else if (i_adh_pch)
        r_pchs = i_adh;
    else 
        r_pchs = 8'h0; 
end

// Increment Logic
always @(*)
begin
    if (i_pclc)
        r_pchs_inc = r_pchs + 1;
    else 
        r_pchs_inc = r_pchs;
end

// Program counter High Register
always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
        r_pch <= 0;
    else if (i_clk_en)
        r_pch <= r_pchs_inc;
end

assign o_pch =  r_pch;

endmodule
