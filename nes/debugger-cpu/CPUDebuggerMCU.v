module CPUDebuggerMCU(
    input i_clk,
    input i_reset_n,

    // CPU memory access
    /* verilator lint_off SYNCASYNCNET */
    input i_cpu_en,
    /* verilator lint_on SYNCASYNCNET */
    input i_cpu_rw,
    input [15:0] i_cpu_address,
    input [7:0] i_cpu_data,
    output reg [7:0] o_cpu_data,

    // Debugger memory access
    input i_debugger_en,
    input i_debugger_rw,
    input [15:0] i_debugger_address,
    input [7:0] i_debugger_data,
    output reg [7:0] o_debugger_data,

    // memory
    output reg o_mem_en,
    output reg o_mem_wea,
    output reg [15:0] o_mem_address,
    output reg [7:0] o_mem_data,
    input [7:0] i_mem_data
);

localparam RW_WRITE = 0;
localparam RW_READ = 1;

reg [7:0] r_debugger_data;
reg [7:0] r_cpu_data;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_debugger_data <= 0;
        r_cpu_data <= 0;
    end
    else
    begin
        if (i_debugger_en)
        begin
            r_debugger_data <= i_mem_data;
        end
        else if (i_cpu_en)
        begin
            r_cpu_data <= i_mem_data;
        end
    end
end

always @(*)
begin
    o_mem_en = 1;

    o_debugger_data = r_debugger_data;
    o_cpu_data = r_cpu_data;
    
    if (i_debugger_en)
    begin
        // debugger takes precedence over CPU

        o_mem_wea = (i_debugger_rw == RW_WRITE);
        o_mem_address = i_debugger_address;
        o_debugger_data = i_mem_data;
        o_mem_data = i_debugger_data;
    end
    else if (i_cpu_en)
    begin
        o_mem_wea = (i_cpu_rw == RW_WRITE);
        o_mem_address = i_cpu_address;
        o_cpu_data = i_mem_data;
        o_mem_data = i_cpu_data;
    end
end

endmodule