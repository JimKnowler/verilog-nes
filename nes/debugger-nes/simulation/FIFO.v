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

// TODO: implement simulation

endmodule
