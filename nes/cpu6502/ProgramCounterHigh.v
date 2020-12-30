// ProgramCounterHigh
//
// combination of components for Program Counter high byte
// - Program Counter High Select (PCHS)
// - Increment logic
// - Program Counter High Register (PCH)

module ProgramCounterHigh(
    input i_clk,
    input i_reset_n,        // required for gtestverilog

    // program counter high select register
    input i_pch_pch,        // control signal - input from PCH
    input i_adh_pch,        // control signal - input from ADH
    input [7:0] i_adh,
    
    // increment logic
    input i_pclc,           // control signal - carry in from PCL

    // program counter high register
    output o_pch

    // todo: externally in bus routing
    // input i_pch_adh      // control signal - output to ADH
    // input i_pch_db       // control signal - output to DB
);

wire [7:0] w_pchs;          // output of PCHS
wire [7:0] w_pchs_inc;      // output of increment logic
reg [7:0] r_pch;            // output of PCH

// PCHS
always @(i_pch_pch or i_adh_pch or i_adh or r_pch)
begin
    if (i_pch_pch)
        w_pchs = r_pch;
    else if (i_adh_pch)
        w_pchs = i_adh;
    else 
        w_pchs = 8'h0; 
end

// Increment Logic
always @(w_pchs or i_pclc)
begin
    w_pchs_inc = w_pchs + 1 + i_pcls;
end

// Program counter High Register
always @(posedge i_clk)
begin
    r_pch <= w_pchs_inc;
end

output o_pch = r_pch;

endmodule
