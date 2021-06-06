module CPUMemoryMap(
    /* verilator lint_off UNUSED */
    /* verilator lint_off UNDRIVEN */    
    input i_clk,
    input i_reset_n,

    // connections to CPU
    input i_clk_en,
    input [15:0] i_address_cpu,
    input i_rw_cpu,
    input [7:0] i_data_cpu,
    output [7:0] o_data_cpu,

    // connections to RAM (read/write)
    output o_cs_ram,
    output [15:0] o_address_ram,            // 0x0000 to 0x07FF (2KB)
    output o_rw_ram,                        // 1 = READ, 0 = WRITE
    output [7:0] o_data_ram,
    input [7:0] i_data_ram,

    // connections to PRG (read)
    output o_cs_prg,
    output [15:0] o_address_prg,
    output [7:0] i_data_prg,

    // connections to PPU (read/write)
    output o_cs_ppu,
    output [2:0] o_rs_ppu,
    output [7:0] o_data_ppu,
    input [7:0] i_data_ppu,
    output o_rw_ppu
    /* verilator lint_on UNUSED */
    /* verilator lint_on UNDRIVEN */
);

localparam [1:0] ADDRESS_RANGE_NONE = 0;
localparam [1:0] ADDRESS_RANGE_RAM = 1;
localparam [1:0] ADDRESS_RANGE_PPU = 2;
localparam [1:0] ADDRESS_RANGE_PRG = 3;

reg [1:0] r_address_range;
reg [15:0] r_address;
reg [7:0] r_data_cpu;

always @(*)
begin
    r_address_range = ADDRESS_RANGE_NONE;
    r_address = 0;
    r_data_cpu = 0;

    if (i_clk_en)
    begin

        if (i_address_cpu < 16'h2000)
        begin
            r_address_range = ADDRESS_RANGE_RAM;
            r_address = i_address_cpu & 16'h7FF;
            r_data_cpu = i_data_ram;
        end
        
        if ((i_address_cpu >= 16'h2000) && (i_address_cpu < 16'h4000))
        begin
            r_address_range = ADDRESS_RANGE_PPU;
            r_data_cpu = i_data_ppu;
            r_address = i_address_cpu & 16'h0007;   // 3 least significant bits
        end
        
        if (i_address_cpu >= 16'h8000)
        begin
            r_address_range = ADDRESS_RANGE_PRG;
            r_address = i_address_cpu;
            r_data_cpu = i_data_prg;
        end
    end
end

assign o_cs_prg = (r_address_range == ADDRESS_RANGE_PRG);
assign o_cs_ram = (r_address_range == ADDRESS_RANGE_RAM);
assign o_cs_ppu = (r_address_range == ADDRESS_RANGE_PPU);
assign o_data_cpu = r_data_cpu;
assign o_rw_ram = (r_address_range == ADDRESS_RANGE_RAM) ? i_rw_cpu : 0;
assign o_address_ram = (r_address_range == ADDRESS_RANGE_RAM) ? r_address : 0;
assign o_address_prg = (r_address_range == ADDRESS_RANGE_PRG) ? r_address : 0;
assign o_rs_ppu = (r_address_range == ADDRESS_RANGE_PPU) ? r_address[2:0] : 0;
assign o_rw_ppu = (r_address_range == ADDRESS_RANGE_PPU) ? i_rw_cpu: 0;

endmodule
