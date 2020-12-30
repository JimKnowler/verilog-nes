// ProgramCounterLow
//
// combination of components for Program Counter low byte
// - Program Counter Low Select (PCLS)
// - Increment logic
// - Program Counter Low Register (PCL)

module ProgramCounterLow(
    input i_clk,
    input i_reset_n,    // required for gtestverilog

    // program counter low select register
    input i_pcl_pcl,    // Control Signal - input from PCL
    input i_adl_pcl,    // Control Signal - input from ADL
    input [7:0] i_adl,  // ADL bus

    // increment logic
    input i_i_pc,       // increment
    output reg o_pclc,      // carry out

    // program counter low register
    output o_pcl
    
    // todo: externally in bus routing
    // input i_pcl_db,     // Control Signal - output to DB
    // input o_pcl_adl     // Control Signal - output to ADL
);

wire [8:0] w_pcls;      // output of PCLS
wire [8:0] w_pcls_inc;  // output of increment logic
                        // NOTE: 8th bit => carry out
reg [7:0] r_pcl;        // output of PCL

assign w_pcls[8] = 0;

// PCLS
always @(i_pcl_pcl or i_adl_pcl or i_adl or r_pcl)
begin
    if (i_pcl_pcl)
        w_pcls[7:0] = r_pcl;
    else if (i_adl_pcl)
        w_pcls[7:0] = i_adl;
    else
        w_pcls[7:0] = 8'h0;
end

// Increment Logic
always @(w_pcls)
begin
    if (i_i_pc)
    begin
        w_pcls_inc = w_pcls + 1;
    end
    else
    begin
        w_pcls_inc = w_pcls;
    end
end

// Carry Out
always @(w_pcls_inc)
begin
    o_pclc = w_pcls_inc[8];
end

// Program Counter Low Register
always @(posedge i_clk)
begin
    r_pcl <= w_pcls_inc[7:0];
end

assign o_pcl = r_pcl;

endmodule