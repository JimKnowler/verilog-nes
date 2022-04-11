module CPUMemoryMap(
    /* verilator lint_off UNUSED */
    /* verilator lint_off UNDRIVEN */    
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */
    /* verilator lint_on UNDRIVEN */

    // connections to CPU
    input i_clk_en,
    input [15:0] i_address_cpu,
    input i_rw_cpu,
    input [7:0] i_data_cpu,                 // data written from CPU
    output [7:0] o_data_cpu,                // data read by CPU

    // connections to RAM (read/write)
    output o_cs_ram,
    output [15:0] o_address_ram,            // 0x0000 to 0x07FF (2KB)
    output o_rw_ram,                        // 1 = READ, 0 = WRITE
    output [7:0] o_data_ram,                // data written to RAM
    input [7:0] i_data_ram,                 // data read from RAM

    // connections to PRG (read)
    output o_cs_prg,
    output [15:0] o_address_prg,
    input [7:0] i_data_prg,                 // data read from PRG

    // connections to PPU (read/write)
    output [2:0] o_rs_ppu,
    output [7:0] o_data_ppu,
    input [7:0] i_data_ppu,
    output o_rw_ppu,

    // connections for controllers
    input i_oe1_n,
    input i_oe2_n,
    input i_controller_1
);

localparam [1:0] ADDRESS_RANGE_NONE = 0;
localparam [1:0] ADDRESS_RANGE_RAM = 1;
localparam [1:0] ADDRESS_RANGE_PPU = 2;
localparam [1:0] ADDRESS_RANGE_PRG = 3;

localparam RW_READ = 1;
localparam RW_WRITE = 0;

reg [1:0] r_address_range;
reg [15:0] r_address;

/* verilator lint_off UNOPTFLAT */
reg [7:0] r_data;
/* verilator lint_on UNOPTFLAT */

always @(*)
begin
    r_address_range = ADDRESS_RANGE_NONE;
    r_address = 0;
    r_data = 0;

    if (i_clk_en)
    begin

        if (i_address_cpu < 16'h2000)
        begin
            r_address_range = ADDRESS_RANGE_RAM;
            r_address = i_address_cpu & 16'h7FF;

            if (i_rw_cpu == RW_READ)
            begin
                r_data = i_data_ram;
            end
            else
            begin
                r_data = i_data_cpu;
            end
        end
        
        if ((i_address_cpu >= 16'h2000) && (i_address_cpu < 16'h4000))
        begin
            r_address_range = ADDRESS_RANGE_PPU;
            r_address = i_address_cpu & 16'h0007;   // 3 least significant bits

            if (i_rw_cpu == RW_READ)
            begin
                r_data = i_data_ppu;
            end
            else
            begin
                r_data = i_data_cpu;
            end
        end
        
        if (i_address_cpu >= 16'h8000)
        begin
            r_address_range = ADDRESS_RANGE_PRG;
            r_address = i_address_cpu;
            if (i_rw_cpu == RW_READ)
            begin
                r_data = i_data_prg;
            end
        end

        if ((i_oe1_n == 0) && (i_rw_cpu == RW_READ))
        begin
            r_data = 8'h40;

            // controller reports 0 for pressed button, inverted here to make a 1
            // https://www.nesdev.org/wiki/Controller_port_pinout
            r_data[0] = ~i_controller_1;
        end

        if ((i_oe2_n == 0) && (i_rw_cpu == RW_READ))
        begin
            // controller 2 defaults to inactive
            r_data = 8'h40;
            r_data[0] = 0;
        end
    end
end

assign o_cs_prg = (r_address_range == ADDRESS_RANGE_PRG);
assign o_cs_ram = (r_address_range == ADDRESS_RANGE_RAM);
assign o_data_cpu = (i_rw_cpu == RW_READ) ? r_data : 0;
assign o_rw_ram = (r_address_range == ADDRESS_RANGE_RAM) ? i_rw_cpu : 0;
assign o_address_ram = (r_address_range == ADDRESS_RANGE_RAM) ? r_address : 0;
assign o_address_prg = (r_address_range == ADDRESS_RANGE_PRG) ? r_address : 0;
assign o_rs_ppu = (r_address_range == ADDRESS_RANGE_PPU) ? r_address[2:0] : 0;
assign o_rw_ppu = (r_address_range == ADDRESS_RANGE_PPU) ? i_rw_cpu: 0;
assign o_data_ram = ((r_address_range == ADDRESS_RANGE_RAM) && (i_rw_cpu == RW_WRITE)) ? r_data : 0;
assign o_data_ppu = ((r_address_range == ADDRESS_RANGE_PPU) && (i_rw_cpu == RW_WRITE)) ? r_data : 0;

endmodule
