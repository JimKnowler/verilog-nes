module VideoOutput
#(
    parameter PIXEL_BITWIDTH = 11,              // x, y co-ords
    parameter RGB_BITWIDTH = 8,                 // r,g,b channels

    parameter PIXEL_RGB_BITWIDTH = RGB_BITWIDTH * 3,

    parameter [8:0] NES_VISIBLE_WIDTH = 255,    // NES video horizontal resolution
    parameter [PIXEL_BITWIDTH-1:0] VGA_VISIBLE_WIDTH = NES_VISIBLE_WIDTH * 2           // render each NES pixel twice
)
(
    input i_clk,
    input i_reset_n,

    // data received from FIFO
    input i_pixel_valid,       
    input [PIXEL_RGB_BITWIDTH-1:0] i_pixel_rgb,

    // driving VGA pixel data
    output o_vga_reset_n,                       // note: used to reset VGA until first line of first frame is ready
    input [PIXEL_BITWIDTH-1:0] i_vga_x,         // x co-ord of the pixel that VGA is currently rendering
    output [RGB_BITWIDTH-1:0] o_vga_red,
    output [RGB_BITWIDTH-1:0] o_vga_green,
    output [RGB_BITWIDTH-1:0] o_vga_blue,

    // debug
    output [8:0] o_debug_linebuffer_write_index,
    output [1:0] o_debug_linebuffer_read,
    output [1:0] o_debug_linebuffer_write,
    output o_debug_linebuffer_read_count,
    output o_debug_vga_visible
);

// line buffers
reg [PIXEL_RGB_BITWIDTH-1:0] r_linebuffer0 [NES_VISIBLE_WIDTH-1:0];
reg [PIXEL_RGB_BITWIDTH-1:0] r_linebuffer1 [NES_VISIBLE_WIDTH-1:0];
reg [PIXEL_RGB_BITWIDTH-1:0] r_linebuffer2 [NES_VISIBLE_WIDTH-1:0];

// default linebuffer values
genvar i;
reg [PIXEL_RGB_BITWIDTH-1:0] r_linebuffer_default [NES_VISIBLE_WIDTH-1:0];
generate
    for (i=0; i<NES_VISIBLE_WIDTH; i=i+1)
    begin
        assign r_linebuffer_default[i] = PIXEL_RGB_BITWIDTH'('h0);
    end
endgenerate

// indexes for the write + read buffer
reg [1:0] r_linebuffer_write;
reg [1:0] r_linebuffer_read;

// how many times has the current read linebuffer been displayed
reg r_linebuffer_read_count;

// write index into the write linebuffer
reg [8:0] r_linebuffer_write_index;

// VGA output
reg [PIXEL_RGB_BITWIDTH-1:0] r_vga_pixel_rgb;
reg r_vga_visible;
reg r_vga_reset_n;

always @(*)
begin
    r_vga_pixel_rgb = 0;
    r_vga_visible = (i_vga_x < VGA_VISIBLE_WIDTH) && (r_vga_reset_n == 1);

    if (r_vga_visible)
        case (r_linebuffer_read)
        0:          r_vga_pixel_rgb = r_linebuffer0[i_vga_x[8:1]];      // NOTE: ignoring LSB, so each pixel rendered twice
        1:          r_vga_pixel_rgb = r_linebuffer1[i_vga_x[8:1]];      // NOTE: ignoring LSB, so each pixel rendered twice
        default:    r_vga_pixel_rgb = r_linebuffer2[i_vga_x[8:1]];
        endcase
    else
        r_vga_pixel_rgb = 0;
end

// internal ring buffer

always @(negedge i_reset_n or posedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_linebuffer_write <= 0;
        r_linebuffer_read <= 0;

        r_linebuffer_read_count <= 0;

        r_linebuffer_write_index <= 0;

        r_linebuffer0 <= r_linebuffer_default;
        r_linebuffer1 <= r_linebuffer_default;
        r_linebuffer2 <= r_linebuffer_default;

        r_vga_reset_n <= 0;
    end
    else
    begin
        // write to the current write linebuffer
        if (i_pixel_valid)
        begin
            if (r_linebuffer_write_index < NES_VISIBLE_WIDTH) 
            begin
                case (r_linebuffer_write)
                0:       r_linebuffer0[r_linebuffer_write_index[7:0]] <= i_pixel_rgb;
                1:       r_linebuffer1[r_linebuffer_write_index[7:0]] <= i_pixel_rgb;
                default: r_linebuffer2[r_linebuffer_write_index[7:0]] <= i_pixel_rgb;
                endcase
            
                r_linebuffer_write_index <= r_linebuffer_write_index + 1;
            end
        end
        
        // during h-sync, prepare to write to the next linebuffer
        if (r_linebuffer_write_index == NES_VISIBLE_WIDTH)
        begin
            // the write buffer is full

            // start writing to the next line
            r_linebuffer_write <= (r_linebuffer_write + 1) % 3;
            
            // reset write index for the back buffer
            r_linebuffer_write_index <= 0;

            // make sure VGA can start rendering, if it hasn't already
            r_vga_reset_n <= 1;
        end

        // read from the current read linebuffer
        if (r_vga_reset_n == 1)
        begin
            // while rendering

            if ((i_vga_x == (VGA_VISIBLE_WIDTH)) && (r_linebuffer_read != r_linebuffer_write))
            begin
                // finished rendering a line

                if (r_linebuffer_read_count == 1)
                begin
                    // we have rendered this linebuffer twice
                    // start rendering the next linebuffer
                    r_linebuffer_read_count <= 0;
                    r_linebuffer_read <= ( r_linebuffer_read + 1) % 3;
                end
                else
                begin
                    // render the current read linebuffer again
                    r_linebuffer_read_count <= 1;
                end
            end

        end
    end
end

assign o_vga_red = r_vga_pixel_rgb[7:0];
assign o_vga_green = r_vga_pixel_rgb[15:8];
assign o_vga_blue = r_vga_pixel_rgb[23:16];
assign o_vga_reset_n = r_vga_reset_n;

assign o_debug_linebuffer_write_index = r_linebuffer_write_index;
assign o_debug_linebuffer_read = r_linebuffer_read;
assign o_debug_linebuffer_write = r_linebuffer_write;
assign o_debug_vga_visible = r_vga_visible;
assign o_debug_linebuffer_read_count = r_linebuffer_read_count;

endmodule
