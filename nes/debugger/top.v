`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 10.11.2021 14:46:09
// Design Name: 
// Module Name: top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module top(
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
    .i_spi_cs_n(i_spi_cs_n)
);

wire [15:0] w_mem_address;
wire w_mem_rw;
wire w_mem_en;
wire [7:0] w_mem_data_wr;
wire [7:0] w_mem_data_rd;

Debugger debugger(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    
    .i_rx_dv(w_rx_dv),
    .i_rx_byte(w_rx_byte),
    
    .o_tx_dv(w_tx_dv),
    .o_tx_byte(w_tx_byte),
    
    .o_mem_address(w_mem_address),
    .o_mem_rw(w_mem_rw),
    .o_mem_en(w_mem_en),
    .o_mem_data(w_mem_data_wr),
    .i_mem_data(w_mem_data_rd)
);

reg r_is_mem_wen;

always @(*)
begin
    r_is_mem_wen = (w_mem_rw == 0);
end

blk_mem_gen_0 memory (
  .clka(i_clk),             // input wire clka
  .ena(w_mem_en),           // input wire ena
  .wea(r_is_mem_wen),       // input wire [0 : 0] wea
  .addra(w_mem_address),    // input wire [15 : 0] addra
  .dina(w_mem_data_wr),     // input wire [7 : 0] dina
  .douta(w_mem_data_rd)     // output wire [7 : 0] douta
);

endmodule
