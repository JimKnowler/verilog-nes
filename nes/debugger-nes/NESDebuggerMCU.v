module NESDebuggerMCU(
    input i_clk,
    input i_reset_n,

    // NES 
    /* verilator lint_off SYNCASYNCNET */
    input i_nes_en,
    /* verilator lint_on SYNCASYNCNET */
    input i_nes_rw,
    input [15:0] i_nes_address,
    input [7:0] i_nes_data,
    output reg [7:0] o_nes_data,

    // Debugger memory access
    input i_debugger_en,
    input i_debugger_rw,
    input [15:0] i_debugger_address,
    input [7:0] i_debugger_data,
    output reg [7:0] o_debugger_data,

    // memory pool 
    output reg o_mem_en,
    output reg o_mem_wea,
    output reg [15:0] o_mem_address,
    output reg [7:0] o_mem_data,
    input [7:0] i_mem_data
);

localparam RW_WRITE = 0;
localparam RW_READ = 1;

reg [7:0] r_debugger_data;
reg [7:0] r_nes_data;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_debugger_data <= 0;
        r_nes_data <= 0;
    end
    else
    begin
        if (i_debugger_en)
        begin
            r_debugger_data <= i_mem_data;
        end
        else if (i_nes_en)
        begin
            r_nes_data <= i_mem_data;
        end
    end
end

always @(*)
begin
    o_mem_en = 1;

    o_mem_wea = 0;
    o_mem_address = 0;
    o_debugger_data = r_debugger_data;
    o_nes_data = r_nes_data;
    o_mem_data = 0;
    
    if (i_debugger_en)
    begin
        // debugger takes precedence over CPU

        o_mem_wea = (i_debugger_rw == RW_WRITE);
        o_mem_address = i_debugger_address;
        o_debugger_data = i_mem_data;
        o_mem_data = i_debugger_data;
    end
    else if (i_nes_en)
    begin
        o_mem_wea = (i_nes_rw == RW_WRITE);
        o_mem_address = i_nes_address;
        o_nes_data = i_mem_data;
        o_mem_data = i_nes_data;
    end
end

endmodule