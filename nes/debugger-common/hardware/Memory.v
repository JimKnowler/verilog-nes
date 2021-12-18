/*
 * 64K RAM implemented as BRAM on ARTY A7
 */
module Memory(
    input i_clk,
    input i_ena,
    input i_wea,
    input [15:0] i_addr,
    input [7:0] i_data,
    output [7:0] o_data
);

blk_mem_gen_0 blockRam (
  .clka(i_clk),      // input wire clka
  .ena(i_ena),       // input wire ena
  .wea(i_wea),       // input wire [0 : 0] wea
  .addra(i_addr),    // input wire [15 : 0] addra
  .dina(i_data),     // input wire [7 : 0] dina
  .douta(o_data)     // output wire [7 : 0] douta
);

endmodule
