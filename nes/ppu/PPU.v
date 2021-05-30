// PPU - Picture Processing Unit
// AKA "2C02"

module PPU(
    input i_clk,
    input i_reset_n,

    // chip select
    input i_cs_n,

    // CPU interface
    output o_int_n,                 // ~Interrupt, to drive ~NMI on CPU
    input [2:0] i_rs,               // register select
    input [7:0] i_data,             // Read from CPU data bus
    output [7:0] o_data,            // Write to CPU data bus
    input i_rw,                     // Read/~Write for CPU data bus

/* verilator lint_off UNUSED */
/* verilator lint_off UNDRIVEN */

    // VIDEO interface    
    output o_video_rd_n,                // ~Read from VIDEO data bus
    output o_video_we_n,                // ~Write to VIDEO data bus
    output [13:0] o_video_address,      // address for video data bus
    output [7:0] o_video_data,          // data write to video data bus
    input [7:0] i_video_data,           // data read from video data bus

    // Video output
    output [7:0] o_video_red,
    output [7:0] o_video_green,
    output [7:0] o_video_blue,

/* verilator lint_on UNDRIVEN */
/* verilator lint_on UNUSED */

    output [8:0] o_video_x,             // pixel clock - x co-ord of current pixel
    output [8:0] o_video_y,             // pixel clock - y co-ord of current pixel
    output o_video_visible,              // pixel clock - visibility of the current pixel

    // debug ports
    output [7:0] o_debug_ppuctrl,
    output [7:0] o_debug_ppumask
);

// Screen Constants
localparam [8:0] SCREEN_WIDTH = 341;
localparam [8:0] SCREEN_HEIGHT = 262;
localparam [8:0] SCREEN_VISIBLE_WIDTH = 256;
localparam [8:0] SCREEN_VISIBLE_HEIGHT = 240;

// RS - register select options
localparam [2:0] RS_PPUCTRL = 0;
localparam [2:0] RS_PPUMASK = 1;
localparam [2:0] RS_PPUSTATUS = 2;

// RW - read / write options
localparam RW_READ = 1;
localparam RW_WRITE = 0;

reg r_int_n;
reg r_video_rd_n;
reg r_video_we_n;

reg [7:0] r_data;
reg [7:0] r_ppuctrl;
reg [7:0] r_ppumask;
reg [6:0] r_ppustatus;          // note: bit 7 is provided by r_nmi_occurred

reg [8:0] r_video_x;
reg [8:0] r_video_y;
wire w_video_visible;

/* verilator lint_off UNUSED */

// OAM sprite data
reg [7:0] r_oam [255:0];

// NMI_Occurred
// - set true when vblank starts
// - set false when vblank ends
// - read through ppustatus[7], and set false after the read
reg r_nmi_occurred;

// NMI_Output
// - PPU pulls o_nmi_n low when nmi_occurred && nmi_output
wire w_nmi_output;
assign w_nmi_output = r_ppuctrl[7];

/* verilator lint_on UNUSED */

//
// READ - PPU/OAM Registers
//

always @(*)
begin
    r_video_rd_n = 1;
    r_video_we_n = 1;
    
    // todo: chip select

    if (i_rw == RW_READ)
    begin
        case (i_rs)
        RS_PPUSTATUS: begin
            r_data = { r_nmi_occurred, r_ppustatus[6:0] };
        end
        default: begin
            r_data = 0;
        end
        endcase
    end
    else
    begin
        r_data = 0;
    end
end

//
// WRITE - PPU/OAM Registers
//

always @(negedge i_reset_n or negedge i_clk)
begin
    if (i_reset_n == 0)
    begin
        r_ppuctrl <= 0;
        r_ppumask <= 0;
        r_ppustatus <= 0;
    end
    else if (i_cs_n == 0)
    begin
        if (i_rw == RW_WRITE)
        begin
            case (i_rs)
            RS_PPUCTRL: begin
                r_ppuctrl <= i_data;
            end
            RS_PPUMASK: begin
                r_ppumask <= i_data;
            end
            default: begin
            end
            endcase
        end
    end
end

//
// NMI for vblank
//

always @(negedge i_reset_n or negedge i_clk)
begin
    if (i_reset_n == 0)
    begin
        r_nmi_occurred <= 0;
    end
    else
    begin
        if ((i_rw == RW_READ) && (i_rs == RS_PPUSTATUS))
        begin
            r_nmi_occurred <= 0;
        end
        else if ((r_video_x == 0) && (r_video_y == 242))
        begin
            // start of vblank
            r_nmi_occurred <= 1;
        end
        else if ((r_video_x == (SCREEN_WIDTH-1)) && (r_video_y == (SCREEN_HEIGHT-1)))
        begin
            // end of vblank
            r_nmi_occurred <= 0;
        end
    end
end

always @(*)
begin
    r_int_n = !(r_nmi_occurred & w_nmi_output);
end

//
// Pixel Clock
//

always @(negedge i_reset_n or negedge i_clk)
begin
    if (i_reset_n == 0)
    begin
        r_video_x <= -1;
        r_video_y <= 0;        
    end
    else
    begin
        if (r_video_x != (SCREEN_WIDTH-1))
        begin
            r_video_x <= r_video_x + 1;
        end
        else
        begin
            r_video_x <= 0;

            if (r_video_y != (SCREEN_HEIGHT-1))
            begin
                r_video_y <= r_video_y + 1;
            end
            else
            begin
                r_video_y <= 0;
            end
        end


    end
end

assign w_video_visible = (r_video_x < 256) && (r_video_y < 240);

//
// drive outputs
//

assign o_int_n = r_int_n;
assign o_data = r_data;

assign o_video_rd_n = r_video_rd_n;
assign o_video_we_n = r_video_we_n;
assign o_video_x = r_video_x;
assign o_video_y = r_video_y;
assign o_video_visible = w_video_visible;

assign o_debug_ppuctrl = r_ppuctrl;
assign o_debug_ppumask = r_ppumask;

endmodule