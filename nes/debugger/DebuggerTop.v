/*
 * Top module for 6502 debugger
 */

module DebuggerTop(
    input   i_clk,
    input   i_reset_n,
    
    // SPI interface
    input           i_spi_cs_n,
    input           i_spi_clk,      // SPI CLK: clock signal from controller
    output          o_spi_cipo,     // SPI CIPO: tri-state in top module: high-z when cs is positive
    input           i_spi_copi      // SPI CPOI: only process when cs is negative
);

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
/* verilator lint_on UNUSED */

SPIPeripheral spi(
    .i_clk(i_clk),
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
    .o_debug_active(w_debug_active)
);

wire [15:0] w_mem_address;
wire w_mem_rw;
wire w_mem_en;
wire [7:0] w_mem_data_wr;
wire [7:0] w_mem_data_rd;

// unused DEBUG pins
/* verilator lint_off UNUSED */
wire [7:0] w_debug_cmd;
wire [15:0] w_debug_cmd_bytes_remaining;
/* verilator lint_on UNUSED */

Debugger debugger(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n & ~i_spi_cs_n),
    
    .i_rx_dv(w_rx_dv),
    .i_rx_byte(w_rx_byte),
    
    .o_tx_dv(w_tx_dv),
    .o_tx_byte(w_tx_byte),
    
    .o_mem_address(w_mem_address),
    .o_mem_rw(w_mem_rw),
    .o_mem_en(w_mem_en),
    .o_mem_data(w_mem_data_wr),
    .i_mem_data(w_mem_data_rd),

    .o_debug_cmd(w_debug_cmd),
    .o_debug_cmd_bytes_remaining(w_debug_cmd_bytes_remaining)
);

reg r_is_mem_wen;

always @(*)
begin
    r_is_mem_wen = (w_mem_rw == 0);
end

Memory memory (
  .i_clk(i_clk),              // input wire clka
  .i_ena(w_mem_en),           // input wire ena
  .i_wea(r_is_mem_wen),       // input wire [0 : 0] wea
  .i_addr(w_mem_address),     // input wire [15 : 0] addra
  .i_data(w_mem_data_wr),     // input wire [7 : 0] dina
  .o_data(w_mem_data_rd)      // output wire [7 : 0] douta
);

endmodule
