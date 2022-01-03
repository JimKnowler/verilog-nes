/*
 * FIFO for CDC VideoOutput
 * input: 8bit R,G,B pixel values from NES in 5mhz clock domain
 * output: 24bit combined RGB pixel value in VGA clock domain
 */

module FIFO(
    input i_clk_5mhz,
    input i_clk_25mhz,
    input i_reset_n,
    
    input i_video_valid,
    input [7:0] i_video_red,
    input [7:0] i_video_green,
    input [7:0] i_video_blue,

    output o_pixel_valid,
    output [23:0] o_pixel_rgb
);

wire [23:0] w_video_output;
wire w_full;
wire w_empty;

wire w_wr_rst_busy;
wire w_rd_rst_busy;

fifo_generator_0 fifo_ip (
  .rst(i_reset_n),
  .wr_clk(i_clk_5mhz),
  .rd_clk(i_clk_25mhz),
  .din({i_video_red, i_video_green, i_video_blue}),
  .wr_en(i_video_visible),
  .rd_en(1),
  .dout(w_video_output),
  .full(w_full),
  .empty(w_empty),
  .wr_rst_busy(w_wr_rst_busy),
  .rd_rst_busy(w_rd_rst_busy)
);

assign o_pixel_valid = !w_empty;
assign o_pixel_rgb  = w_video_output;

endmodule
