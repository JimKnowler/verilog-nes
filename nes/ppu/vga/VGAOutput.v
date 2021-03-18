// VGAOutput
//
// - Combine signals from VGAGeneator and an RGB Video Renderer to create output VGA signals
// - Pure combinatorial logic
// - Parameters default for VGA 640x480 @ 60Hz  (25.175MHz pixel clock)
//   http://tinyvga.com/vga-timing/640x480@60Hz

module VGAOutput
#(
    parameter WIDTH = 800,   
    parameter HEIGHT = 525,
    parameter WIDTH_VISIBLE = 640,
    parameter HEIGHT_VISIBLE = 480,
    parameter FRONT_PORCH_X = 16,
    parameter BACK_PORCH_X = 96,
    parameter FRONT_PORCH_Y = 10,
    parameter BACK_PORCH_Y = 33,

    parameter PIXEL_BITWIDTH = 11,
    parameter RGB_BITWIDTH = 8
)  
(
    /* verilator lint_off UNUSED */
    input i_clk,                            // only required to 
    input i_reset_n,                        //  generate testbench
    /* verilator lint_on UNUSED */

    // input from VGAGenerator
    input i_visible,
    input [PIXEL_BITWIDTH-1:0] i_x,
    input [PIXEL_BITWIDTH-1:0] i_y,

    // input from RGB Video Renderer
    input [RGB_BITWIDTH-1:0] i_red,
    input [RGB_BITWIDTH-1:0] i_green,
    input [RGB_BITWIDTH-1:0] i_blue,
    
    // output VGA signals
    output [RGB_BITWIDTH-1:0] o_vga_red,
    output [RGB_BITWIDTH-1:0] o_vga_green,
    output [RGB_BITWIDTH-1:0] o_vga_blue,
    output o_vga_hsync,
    output o_vga_vsync
);
    reg r_hsync;
    reg r_vsync;

    always @(*)
    begin
        r_hsync = (i_x < (WIDTH_VISIBLE + FRONT_PORCH_X)) || (i_x >= (WIDTH - BACK_PORCH_X));
        r_vsync = (i_y < (HEIGHT_VISIBLE + FRONT_PORCH_Y)) || (i_y >= (HEIGHT - BACK_PORCH_Y));
    end

    assign o_vga_red[RGB_BITWIDTH-1:0] = i_visible ? i_red[RGB_BITWIDTH-1:0] : 0;
    assign o_vga_green[RGB_BITWIDTH-1:0] = i_visible ? i_green[RGB_BITWIDTH-1:0] : 0;
    assign o_vga_blue[RGB_BITWIDTH-1:0] = i_visible ? i_blue[RGB_BITWIDTH-1:0] : 0;

    assign o_vga_hsync = r_hsync;
    assign o_vga_vsync = r_vsync;
endmodule
