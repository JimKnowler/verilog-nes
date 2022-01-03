module VideoOutput
#(
    parameter PIXEL_BITWIDTH = 11,              // x, y co-ords
    parameter RGB_BITWIDTH = 8,                 // r,g,b channels

    parameter PIXEL_RGB_BITWIDTH = RGB_BITWIDTH * 3,

    parameter [8:0] NES_VISIBLE_WIDTH = 256,    // NES video horizontal resolution
    parameter [PIXEL_BITWIDTH-1:0] VGA_VISIBLE_WIDTH = NES_VISIBLE_WIDTH * 2           // render each NES pixel twice
)
(
    input i_clk,
    input i_reset_n,

    // data received from FIFO
    input i_pixel_valid,       
    input [PIXEL_RGB_BITWIDTH-1:0] i_pixel_rgb,

    // driving VGA pixel data
    output o_vga_reset_n,                       // note: used to reset VGA until 
    input [PIXEL_BITWIDTH-1:0] i_vga_x,         // x co-ord of the pixel that VGA is currently rendering
    output [RGB_BITWIDTH-1:0] o_vga_red,
    output [RGB_BITWIDTH-1:0] o_vga_green,
    output [RGB_BITWIDTH-1:0] o_vga_blue,

    // debug
    output [8:0] o_debug_linebuffer_write_index,
    output o_debug_linebuffer_front,
    output o_debug_vga_visible
);

// front and back line buffers
reg [PIXEL_RGB_BITWIDTH-1:0] r_linebuffer0 [NES_VISIBLE_WIDTH-1:0];
reg [PIXEL_RGB_BITWIDTH-1:0] r_linebuffer1 [NES_VISIBLE_WIDTH-1:0];

// default linebuffer values
genvar i;
reg [PIXEL_RGB_BITWIDTH-1:0] r_linebuffer_default [NES_VISIBLE_WIDTH-1:0];
generate
    for (i=0; i<NES_VISIBLE_WIDTH; i=i+1)
    begin
        assign r_linebuffer_default[i] = PIXEL_RGB_BITWIDTH'('h0);
    end
endgenerate

// which line buffer is currently used as the front buffer
// (note: back line buffer index is just 1-r_linebuffer_front)
reg r_linebuffer_front;

// write index into the back linebuffer
reg [8:0] r_linebuffer_write_index;

// VGA output
reg [PIXEL_RGB_BITWIDTH-1:0] r_vga_pixel_rgb;
reg r_vga_visible;
reg r_vga_reset_n;

always @(negedge i_reset_n or posedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_linebuffer_front <= 0;
        r_linebuffer_write_index <= 0;

        r_linebuffer0 <= r_linebuffer_default;
        r_linebuffer1 <= r_linebuffer_default;

        r_vga_reset_n <= 0;
    end
    else
    begin
        if (i_pixel_valid)
        begin
            if (r_linebuffer_write_index < NES_VISIBLE_WIDTH) 
            begin
                case (r_linebuffer_front)
                0:       r_linebuffer1[r_linebuffer_write_index[7:0]] <= i_pixel_rgb;
                default: r_linebuffer0[r_linebuffer_write_index[7:0]] <= i_pixel_rgb;
                endcase
            
                r_linebuffer_write_index <= r_linebuffer_write_index + 1;
            end
        end
        else if (r_linebuffer_write_index == NES_VISIBLE_WIDTH)
        begin
            // the back buffer is full

            if (!r_vga_visible) 
            begin
                // swap front/back buffers
                r_linebuffer_front <= r_linebuffer_front ? 0 : 1;
                
                // reset write index for the back buffer
                r_linebuffer_write_index <= 0;

                // make sure VGA can start rendering, if it hasn't already
                r_vga_reset_n <= 1;
            end
        end
    end
end

always @(*)
begin
    r_vga_pixel_rgb = 0;
    r_vga_visible = (i_vga_x < VGA_VISIBLE_WIDTH) && (r_vga_reset_n == 1);

    if (r_vga_visible)
    case (r_linebuffer_front)
    0:          r_vga_pixel_rgb = r_linebuffer0[i_vga_x[8:1]];      // NOTE: ignoring LSB, so each pixel rendered twice
    default:    r_vga_pixel_rgb = r_linebuffer1[i_vga_x[8:1]];
    endcase
end

assign o_vga_red = r_vga_pixel_rgb[7:0];
assign o_vga_green = r_vga_pixel_rgb[15:8];
assign o_vga_blue = r_vga_pixel_rgb[23:16];
assign o_vga_reset_n = r_vga_reset_n;

assign o_debug_linebuffer_write_index = r_linebuffer_write_index;
assign o_debug_linebuffer_front = r_linebuffer_front;
assign o_debug_vga_visible = r_vga_visible;

endmodule
