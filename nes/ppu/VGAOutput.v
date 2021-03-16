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
    parameter WIDTH_ACTIVE = 640,
    parameter HEIGHT_ACTIVE = 480,
    parameter FRONT_PORCH_X = 16,
    parameter BACK_PORCH_X = 96,
    parameter FRONT_PORCH_Y = 10,
    parameter BACK_PORCH_Y = 33
)  
(
    // input from VGAGenerator
    input i_active,
    input [10:0] i_x,
    input [10:0] i_y,

    // input from RGB Video Renderer
    input [7:0] i_red,
    input [7:0] i_green,
    input [7:0] i_blue,
    
    // output VGA signals
    output [7:0] o_vga_red,
    output [7:0] o_vga_green,
    output [7:0] o_vga_blue,
    output o_vga_hsync,
    output vga_o_vsync
);
    reg r_hsync;
    reg r_vsync;

    always @(*)
    begin
        r_hsync = (i_x < (WIDTH_ACTIVE + FRONT_PORCH_X)) || (i_x >= (WIDTH - BACK_PORCH_X));
        r_vsync = (i_y < (HEIGHT_ACTIVE + FRONT_PORCH_Y)) || (i_y >= (HEIGHT - BACK_PORCH_Y));
    end

    assign o_vga_red[2:0] = i_active ? i_red[2:0] : 0;
    assign o_vga_green[2:0] = i_active ? i_green[2:0] : 0;
    assign o_vga_blue[2:0] = i_active ? i_blue[2:0] : 0;

    assign o_vga_hsync = r_hsync;
    assign o_vga_vsync = r_vsync;
endmodule
