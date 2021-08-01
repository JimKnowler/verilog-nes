module PPUMemoryMap(
    /* verilator lint_off UNUSED */
    /* verilator lint_off UNDRIVEN */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */
    /* verilator lint_on UNDRIVEN */

    input [13:0] i_address_ppu,     // address that PPU is accessing
    input i_rd_en_ppu_n,            // set to 0 when PPU is reading
    input i_wr_en_ppu_n,            // set to 0 when PPU is writing
    output [7:0] o_data_ppu,        // data output to PPU
    input [7:0] i_data_ppu,          // data input from PPU

    output o_cs_patterntable,       // 1 when patterntable is selected
    input [7:0] i_data_patterntable,// data read from patterntable
    output o_rw_patterntable,       // 1 = READ, 0 = WRITE   
    output [13:0] o_address_patterntable,   // address output to patterntable

    output o_cs_nametable,          // 1 when nametable is selected
    input [7:0] i_data_nametable,   // data read from nametable
    output [7:0] o_data_nametable,   // data written to nametable
    output o_rw_nametable,          // 1 = READ, 0 = WRITE
    output [13:0] o_address_nametable       // address output to nametable
);

reg address_range;
reg [7:0] r_data;
reg [13:0] r_address;

localparam ADDRESS_RANGE_PATTERNTABLE = 0;
localparam ADDRESS_RANGE_NAMETABLE = 1;

localparam RW_READ = 1;
localparam RW_WRITE = 0;

localparam ADDRESS_NAMETABLE_START = 14'h2000;

always @(*)
begin
    if (i_address_ppu < ADDRESS_NAMETABLE_START)
    begin
        address_range = ADDRESS_RANGE_PATTERNTABLE;
        r_data = i_data_patterntable;
        r_address = i_address_ppu;
    end
    else
    begin
        address_range = ADDRESS_RANGE_NAMETABLE;
        r_data = i_data_nametable;
        r_address = i_address_ppu & 14'h2FFF;
    end
end

wire w_ppu_wr_or_rd_en = (i_rd_en_ppu_n == 0) || (i_wr_en_ppu_n == 0);

assign o_cs_patterntable = (address_range == ADDRESS_RANGE_PATTERNTABLE) && w_ppu_wr_or_rd_en;
assign o_cs_nametable = (address_range == ADDRESS_RANGE_NAMETABLE) && w_ppu_wr_or_rd_en;
assign o_data_ppu = r_data;
assign o_address_patterntable = r_address;
assign o_rw_patterntable = (i_rd_en_ppu_n == 0) ? RW_READ : RW_WRITE;
assign o_address_nametable = r_address;
assign o_rw_nametable = (i_rd_en_ppu_n == 0) ? RW_READ : RW_WRITE;
assign o_data_nametable = i_data_ppu;

endmodule
