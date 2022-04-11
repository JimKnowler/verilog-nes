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

// cache debugger mem read at posedge
reg [7:0] r_debugger_data_read;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_debugger_data_read <= 0;
    end
    else
    begin
        if (i_debugger_en)
        begin
            r_debugger_data_read <= i_mem_data;
        end
    end
end

// cache NES read/write at negedge
reg [7:0] r_nes_read_data;
reg r_nes_write_en;
reg [7:0] r_nes_write_data;
reg [15:0] r_nes_write_address;

always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_nes_read_data <= 0;

        r_nes_write_en <= 0;
        r_nes_write_data <= 0;
        r_nes_write_address <= 0;
    end
    else
    begin
        if (i_nes_en && !i_debugger_en)
        begin
            r_nes_read_data <= i_mem_data;
                        
            if (i_nes_rw == RW_WRITE)
            begin
                r_nes_write_en <= 1;
                r_nes_write_data <= i_nes_data;
                r_nes_write_address <= i_nes_address;
            end
            else
            begin
                r_nes_write_en <= 0;
            end
        end
        else
        begin
            r_nes_write_en <= 0;
        end
    end
end

always @(*)
begin
    o_mem_en = 1;
    o_mem_wea = 0;
    o_mem_address = 0;
    o_debugger_data = r_debugger_data_read;
    o_nes_data = r_nes_read_data;
    o_mem_data = 0;
    
    if (i_debugger_en)
    begin
        // debugger takes precedence over CPU
        o_mem_wea = (i_debugger_rw == RW_WRITE);
        o_mem_address = i_debugger_address;
        o_debugger_data = i_mem_data;
        o_mem_data = i_debugger_data;
    end
    else if (r_nes_write_en)
    begin
        o_mem_wea = 1;
        o_mem_address = r_nes_write_address;
        o_mem_data = r_nes_write_data;
    end
    else if (i_nes_en)
    begin
        o_mem_wea = (i_nes_rw == RW_WRITE);
        o_mem_address = i_nes_address;
        o_nes_data = i_mem_data;
        o_mem_data = i_nes_data;
    end
    else
    begin
        o_mem_en = 0;
    end
end

endmodule
