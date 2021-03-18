module VGAExample
#(
    parameter WIDTH = 200,   
    parameter HEIGHT = 150,
    parameter WIDTH_VISIBLE = 150,
    parameter HEIGHT_VISIBLE = 100,
    parameter FRONT_PORCH_X = 20,
    parameter BACK_PORCH_X = 12,
    parameter FRONT_PORCH_Y = 10,
    parameter BACK_PORCH_Y = 5,

    parameter PIXEL_BITWIDTH = 11,
    parameter RGB_BITWIDTH = 8
)
(
    input i_clk,
    input i_reset_n,

    input [PIXEL_BITWIDTH-1:0] i_rect_x,
    input [PIXEL_BITWIDTH-1:0] i_rect_y,
    input [PIXEL_BITWIDTH-1:0] i_rect_width,
    input [PIXEL_BITWIDTH-1:0] i_rect_height,

    output [PIXEL_BITWIDTH-1:0] o_vga_x,
    output [PIXEL_BITWIDTH-1:0] o_vga_y,
    output [RGB_BITWIDTH-1:0] o_vga_red,
    output [RGB_BITWIDTH-1:0] o_vga_green,
    output [RGB_BITWIDTH-1:0] o_vga_blue,
    output o_vga_hsync,
    output o_vga_vsync
);

    wire [PIXEL_BITWIDTH-1:0] w_vga_x;
    wire [PIXEL_BITWIDTH-1:0] w_vga_y;
    wire w_vga_visible;

    VGAGenerator #(
        .WIDTH(WIDTH),
        .HEIGHT(HEIGHT),
        .WIDTH_VISIBLE(WIDTH_VISIBLE),
        .PIXEL_BITWIDTH(PIXEL_BITWIDTH)
    ) vga_generator (
        .i_clk(i_clk),
        .i_reset_n(i_reset_n),
        .o_x(w_vga_x),
        .o_y(w_vga_y),
        .o_visible(w_vga_visible)
    );

    reg [RGB_BITWIDTH-1:0] r_rect_colour;

    wire w_is_rendering_rect = 
        (w_vga_x >= i_rect_x) &&
        (w_vga_y >= i_rect_y) &&
        (w_vga_x < (i_rect_x + i_rect_width)) &&
        (w_vga_y < (i_rect_y + i_rect_height));

    always @(*)
    begin
        r_rect_colour = (w_is_rendering_rect) ? ~0 : 0;
    end

    wire [RGB_BITWIDTH-1:0] w_vga_red;
    wire [RGB_BITWIDTH-1:0] w_vga_green;
    wire [RGB_BITWIDTH-1:0] w_vga_blue;
    wire w_vga_hsync;
    wire w_vga_vsync;

    VGAOutput #(
        .WIDTH(WIDTH),
        .HEIGHT(HEIGHT),
        .WIDTH_VISIBLE(WIDTH_VISIBLE),
        .HEIGHT_VISIBLE(HEIGHT_VISIBLE),
        .FRONT_PORCH_X(FRONT_PORCH_X),
        .BACK_PORCH_X(BACK_PORCH_X),
        .FRONT_PORCH_Y(FRONT_PORCH_Y),
        .BACK_PORCH_Y(BACK_PORCH_Y),
        .PIXEL_BITWIDTH(PIXEL_BITWIDTH),
        .RGB_BITWIDTH(RGB_BITWIDTH)
    ) vga_output (
        .i_clk(i_clk),
        .i_reset_n(i_reset_n),
        .i_visible(w_vga_visible),
        .i_x(w_vga_x),
        .i_y(w_vga_y),
        .i_red(r_rect_colour),
        .i_green(r_rect_colour),
        .i_blue(r_rect_colour),
        .o_vga_red(w_vga_red),
        .o_vga_green(w_vga_green),
        .o_vga_blue(w_vga_blue),
        .o_vga_hsync(w_vga_hsync),
        .o_vga_vsync(w_vga_vsync)
    );

    assign o_vga_x = w_vga_x;
    assign o_vga_y = w_vga_y;
    assign o_vga_red = w_vga_red;
    assign o_vga_green = w_vga_green;
    assign o_vga_blue = w_vga_blue;
    assign o_vga_hsync = w_vga_hsync;
    assign o_vga_vsync = w_vga_vsync;

endmodule