/*
 * Top module for NES debugger
 */

module NESDebuggerTop(
    input   i_clk_5mhz,
    input   i_reset_n,
    
    // SPI interface
    input           i_spi_cs_n,
    input           i_spi_clk,      // SPI CLK: clock signal from controller
    output          o_spi_cipo,     // SPI CIPO: tri-state in top module: high-z when cs is positive
    input           i_spi_copi      // SPI CPOI: only process when cs is negative
);

localparam RW_WRITE = 0;
localparam RW_READ = 1;

wire [7:0] w_rx_byte;
wire w_rx_dv;

wire [7:0] w_tx_byte;
wire w_tx_dv;

// unused DEBUG pins
/* verilator lint_off UNUSED */
wire w_debug_rx_buffered_0;
wire w_debug_rx_buffered_1;
wire w_debug_rx_buffered_2;
wire [2:0] w_debug_tx_bit_index;
wire [2:0] w_debug_rx_bit_index;
wire w_debug_active;
wire [7:0] w_debug_tx_byte_buffered;
/* verilator lint_on UNUSED */

SPIPeripheral spi(
    .i_clk(i_clk_5mhz),
    .i_reset_n(i_reset_n),
    
    .o_rx_byte(w_rx_byte),
    .o_rx_dv(w_rx_dv),
    .i_tx_dv(w_tx_dv),
    .i_tx_byte(w_tx_byte),

    .i_spi_clk(i_spi_clk),
    .o_spi_cipo(o_spi_cipo),
    .i_spi_copi(i_spi_copi),
    .i_spi_cs_n(i_spi_cs_n),

    .o_debug_rx_buffered_0(w_debug_rx_buffered_0),
    .o_debug_rx_buffered_1(w_debug_rx_buffered_1),
    .o_debug_rx_buffered_2(w_debug_rx_buffered_2),
    .o_debug_tx_bit_index(w_debug_tx_bit_index),
    .o_debug_rx_bit_index(w_debug_rx_bit_index),
    .o_debug_active(w_debug_active),
    .o_debug_tx_byte_buffered(w_debug_tx_byte_buffered)
);

wire [15:0] w_debugger_mem_address;
wire w_debugger_mem_rw;
wire w_debugger_mem_en;
wire [7:0] w_debugger_mem_data_wr;
reg [7:0] r_debugger_mem_data_rd;

wire [15:0] w_value_id;
wire w_value_rw;
wire w_value_en;
wire [15:0] w_value_data_wr;
wire [15:0] w_value_data_rd;

// unused DEBUG pins
/* verilator lint_off UNUSED */
wire [7:0] w_debug_cmd;
wire [15:0] w_debug_cmd_bytes_remaining;
/* verilator lint_on UNUSED */

NESDebugger debugger(
    .i_clk(i_clk_5mhz),
    .i_reset_n(i_reset_n & ~i_spi_cs_n),
    
    .i_rx_dv(w_rx_dv),
    .i_rx_byte(w_rx_byte),
    
    .o_tx_dv(w_tx_dv),
    .o_tx_byte(w_tx_byte),
    
    .o_mem_address(w_debugger_mem_address),
    .o_mem_rw(w_debugger_mem_rw),
    .o_mem_en(w_debugger_mem_en),
    .o_mem_data(w_debugger_mem_data_wr),
    .i_mem_data(r_debugger_mem_data_rd),

    .o_value_id(w_value_id),
    .o_value_rw(w_value_rw),
    .o_value_en(w_value_en),
    .o_value_data(w_value_data_wr),
    .i_value_data(w_value_data_rd),

    .o_debug_cmd(w_debug_cmd),
    .o_debug_cmd_bytes_remaining(w_debug_cmd_bytes_remaining)
);

//
// Memory Pool Wiring
//

// wire used by debugger to select which memory pool it is accessing
wire [1:0] w_debugger_memory_pool;
localparam MEMORY_POOL_PRG = 0;
localparam MEMORY_POOL_RAM = 1;
localparam MEMORY_POOL_PATTERNTABLE = 2;
localparam MEMORY_POOL_NAMETABLE = 3;

wire [7:0] w_debugger_mem_prg_data_rd;
wire [7:0] w_debugger_mem_ram_data_rd;
wire [7:0] w_debugger_mem_patterntable_data_rd;
wire [7:0] w_debugger_mem_nametable_data_rd;

always @(*)
begin

    case (w_debugger_memory_pool)
    MEMORY_POOL_PRG: r_debugger_mem_data_rd = w_debugger_mem_prg_data_rd;
    MEMORY_POOL_RAM: r_debugger_mem_data_rd = w_debugger_mem_ram_data_rd;
    MEMORY_POOL_PATTERNTABLE: r_debugger_mem_data_rd = w_debugger_mem_patterntable_data_rd;
    MEMORY_POOL_NAMETABLE: r_debugger_mem_data_rd = w_debugger_mem_nametable_data_rd;
    default: r_debugger_mem_data_rd = 0;
    endcase
end

// PRG - CPU6502 program
wire w_mem_prg_en;
wire w_mem_prg_wea;
wire [15:0] w_mem_prg_address;
wire [7:0] w_mem_prg_data_rd;
wire [7:0] w_mem_prg_data_wr;

wire w_nes_prg_en;
wire w_nes_prg_rw;
wire [15:0] w_nes_prg_address;
wire [7:0] w_nes_prg_data_rd;
wire [7:0] w_nes_prg_data_wr;

assign w_nes_prg_rw = RW_READ;
assign w_nes_prg_data_wr = 0;

// RAM - R/W RAM for CPU6502
wire w_mem_ram_en;
wire w_mem_ram_wea;
wire [15:0] w_mem_ram_address;
wire [7:0] w_mem_ram_data_rd;
wire [7:0] w_mem_ram_data_wr;

wire w_nes_ram_en;
wire w_nes_ram_rw;
wire [15:0] w_nes_ram_address;
wire [7:0] w_nes_ram_data_rd;
wire [7:0] w_nes_ram_data_wr;

// PATTERNTABLE - graphics data for PPU
wire w_mem_patterntable_en;
wire w_mem_patterntable_wea;
wire [15:0] w_mem_patterntable_address;
wire [7:0] w_mem_patterntable_data_rd;
wire [7:0] w_mem_patterntable_data_wr;

wire w_nes_patterntable_en;
wire w_nes_patterntable_rw;
wire [13:0] w_nes_patterntable_address;
wire [7:0] w_nes_patterntable_data_rd;
wire [7:0] w_nes_patterntable_data_wr;

assign w_nes_patterntable_data_wr = 0;

// NAMETABLE - background tile data, managed by PPU
wire w_mem_nametable_en;
wire w_mem_nametable_wea;
wire [15:0] w_mem_nametable_address;
wire [7:0] w_mem_nametable_data_rd;
wire [7:0] w_mem_nametable_data_wr;

wire w_nes_nametable_en;
wire w_nes_nametable_rw;
wire [13:0] w_nes_nametable_address;
wire [7:0] w_nes_nametable_data_rd;
wire [7:0] w_nes_nametable_data_wr;

//
// NES
//

wire w_nes_reset_n;

/* verilator lint_off PINMISSING */
NES nes(
    .i_clk(i_clk_5mhz),
    .i_reset_n(i_reset_n & w_nes_reset_n),

    // video output
    // o_video_red
    // o_video_green
    // o_video_blue
    // o_video_x
    // o_video_y
    // o_video_visible

    // controller
    // o_controller_latch
    // o_controller_clk
    // i_controller_1

    // CPU memory access - RAM
    .o_cs_ram(w_nes_ram_en),
    .o_address_ram(w_nes_ram_address),
    .o_rw_ram(w_nes_ram_rw),
    .o_data_ram(w_nes_ram_data_wr),
    .i_data_ram(w_nes_ram_data_rd),

    // CPU memory access - PRG
    .o_cs_prg(w_nes_prg_en),
    .o_address_prg(w_nes_prg_address),
    .i_data_prg(w_nes_prg_data_rd),

    // PPU memory access - Pattern Table
    .o_cs_patterntable(w_nes_patterntable_en),
    .i_data_patterntable(w_nes_patterntable_data_rd),
    .o_rw_patterntable(w_nes_patterntable_rw),
    .o_address_patterntable(w_nes_patterntable_address),

    // PPU memory access - Nametable
    .o_cs_nametable(w_nes_nametable_en),
    .i_data_nametable(w_nes_nametable_data_rd),
    .o_data_nametable(w_nes_nametable_data_wr),
    .o_rw_nametable(w_nes_nametable_rw),
    .o_address_nametable(w_nes_nametable_address)
);

/* verilator lint_on PINMISSING */

//
// Values - control CPU step + read CPU values
//

reg r_is_value_wea;

always @(*)
begin
    r_is_value_wea = (w_value_rw == RW_WRITE);
end

NESDebuggerValues values (
    .i_clk(i_clk_5mhz),
    .i_reset_n(i_reset_n),

    .i_ena(w_value_en),
    .i_wea(r_is_value_wea),
    .i_id(w_value_id),
    .i_data(w_value_data_wr),
    .o_data(w_value_data_rd),

    .o_nes_reset_n(w_nes_reset_n),
    .o_debugger_memory_pool(w_debugger_memory_pool)
);

//
// Memory
//

NESDebuggerMCU mcu_prg(
    .i_clk(i_clk_5mhz),
    .i_reset_n(i_reset_n),

    // connection to NES
    .i_nes_en(w_nes_prg_en),
    .i_nes_rw(w_nes_prg_rw),
    .i_nes_address(w_nes_prg_address),
    .i_nes_data(w_nes_prg_data_wr),
    .o_nes_data(w_nes_prg_data_rd),

    // connections to debugger
    .i_debugger_en(w_debugger_mem_en && (w_debugger_memory_pool == MEMORY_POOL_PRG)),
    .i_debugger_rw(w_debugger_mem_rw),
    .i_debugger_address(w_debugger_mem_address),
    .i_debugger_data(w_debugger_mem_data_wr),
    .o_debugger_data(w_debugger_mem_prg_data_rd),

    // connections to PRG memory
    .o_mem_en(w_mem_prg_en),
    .o_mem_wea(w_mem_prg_wea),
    .o_mem_address(w_mem_prg_address),
    .o_mem_data(w_mem_prg_data_wr),
    .i_mem_data(w_mem_prg_data_rd)
);

Memory memory_prg (
  .i_clk(i_clk_5mhz),
  .i_ena(w_mem_prg_en),
  .i_wea(w_mem_prg_wea),
  .i_addr(w_mem_prg_address),
  .i_data(w_mem_prg_data_wr),
  .o_data(w_mem_prg_data_rd)
);

NESDebuggerMCU mcu_ram(
    .i_clk(i_clk_5mhz),
    .i_reset_n(i_reset_n),

    // connection to NES
    .i_nes_en(w_nes_ram_en),
    .i_nes_rw(w_nes_ram_rw),
    .i_nes_address(w_nes_ram_address),
    .i_nes_data(w_nes_ram_data_wr),
    .o_nes_data(w_nes_ram_data_rd),

    // connections to debugger
    .i_debugger_en(w_debugger_mem_en && (w_debugger_memory_pool == MEMORY_POOL_RAM)),
    .i_debugger_rw(w_debugger_mem_rw),
    .i_debugger_address(w_debugger_mem_address),
    .i_debugger_data(w_debugger_mem_data_wr),
    .o_debugger_data(w_debugger_mem_ram_data_rd),

    // connections to RAM memory
    .o_mem_en(w_mem_ram_en),
    .o_mem_wea(w_mem_ram_wea),
    .o_mem_address(w_mem_ram_address),
    .o_mem_data(w_mem_ram_data_wr),
    .i_mem_data(w_mem_ram_data_rd)
);

Memory memory_ram (
  .i_clk(i_clk_5mhz),
  .i_ena(w_mem_ram_en),
  .i_wea(w_mem_ram_wea),
  .i_addr(w_mem_ram_address),
  .i_data(w_mem_ram_data_wr),
  .o_data(w_mem_ram_data_rd)
);

NESDebuggerMCU mcu_patterntable(
    .i_clk(i_clk_5mhz),
    .i_reset_n(i_reset_n),

    // connection to NES
    .i_nes_en(w_nes_patterntable_en),
    .i_nes_rw(w_nes_patterntable_rw),
    .i_nes_address({2'b0, w_nes_patterntable_address}),
    .i_nes_data(w_nes_patterntable_data_wr),
    .o_nes_data(w_nes_patterntable_data_rd),

    // connections to debugger
    .i_debugger_en(w_debugger_mem_en && (w_debugger_memory_pool == MEMORY_POOL_PATTERNTABLE)),
    .i_debugger_rw(w_debugger_mem_rw),
    .i_debugger_address(w_debugger_mem_address),
    .i_debugger_data(w_debugger_mem_data_wr),
    .o_debugger_data(w_debugger_mem_patterntable_data_rd),

    // connections to PATTERNTABLE memory
    .o_mem_en(w_mem_patterntable_en),
    .o_mem_wea(w_mem_patterntable_wea),
    .o_mem_address(w_mem_patterntable_address),
    .o_mem_data(w_mem_patterntable_data_wr),
    .i_mem_data(w_mem_patterntable_data_rd)
);

Memory memory_patterntable (
  .i_clk(i_clk_5mhz),
  .i_ena(w_mem_patterntable_en),
  .i_wea(w_mem_patterntable_wea),
  .i_addr(w_mem_patterntable_address),
  .i_data(w_mem_patterntable_data_wr),
  .o_data(w_mem_patterntable_data_rd)
);

NESDebuggerMCU mcu_nametable(
    .i_clk(i_clk_5mhz),
    .i_reset_n(i_reset_n),

    // connection to NES
    .i_nes_en(w_nes_nametable_en),
    .i_nes_rw(w_nes_nametable_rw),
    .i_nes_address({2'b0, w_nes_nametable_address}),
    .i_nes_data(w_nes_nametable_data_wr),
    .o_nes_data(w_nes_nametable_data_rd),

    // connections to debugger
    .i_debugger_en(w_debugger_mem_en && (w_debugger_memory_pool == MEMORY_POOL_NAMETABLE)),
    .i_debugger_rw(w_debugger_mem_rw),
    .i_debugger_address(w_debugger_mem_address),
    .i_debugger_data(w_debugger_mem_data_wr),
    .o_debugger_data(w_debugger_mem_nametable_data_rd),

    // connections to NAMETABLE memory
    .o_mem_en(w_mem_nametable_en),
    .o_mem_wea(w_mem_nametable_wea),
    .o_mem_address(w_mem_nametable_address),
    .o_mem_data(w_mem_nametable_data_wr),
    .i_mem_data(w_mem_nametable_data_rd)
);

Memory memory_nametable (
  .i_clk(i_clk_5mhz),
  .i_ena(w_mem_nametable_en),
  .i_wea(w_mem_nametable_wea),
  .i_addr(w_mem_nametable_address),
  .i_data(w_mem_nametable_data_wr),
  .o_data(w_mem_nametable_data_rd)
);

endmodule
