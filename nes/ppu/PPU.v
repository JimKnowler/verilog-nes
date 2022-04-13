// PPU - Picture Processing Unit
// AKA "2C02"

module PPU(
    input i_clk,
    input i_reset_n,

    // chip select
    input i_cs_n,

    // clock enable
    input i_ce,

    // CPU interface
    output o_int_n,                     // ~Interrupt, to drive ~NMI on CPU
    input [2:0] i_rs,                   // register select
    input [7:0] i_data,                 // Read from CPU data bus
    output [7:0] o_data,                // Write to CPU data bus
    input i_rw,                         // Read/~Write for CPU data bus

    // VRAM interface    
    output o_vram_rd_n,                 // ~Read from VRAM data bus
    output o_vram_we_n,                 // ~Write to VRAM data bus
    output [13:0] o_vram_address,       // address for VRAM data bus
    output [7:0] o_vram_data,           // data write to VRAM data bus
    input [7:0] i_vram_data,            // data read from VRAM data bus

    // Video output
    output [7:0] o_video_red,
    output [7:0] o_video_green,
    output [7:0] o_video_blue,
    output [8:0] o_video_x,             // pixel clock - x co-ord of current pixel
    output [8:0] o_video_y,             // pixel clock - y co-ord of current pixel
    output o_video_visible,             // pixel clock - visibility of the current pixel

    // debug ports
    output [7:0] o_debug_ppuctrl,
    output [7:0] o_debug_ppumask,
    output [7:0] o_debug_ppustatus,
    output [7:0] o_debug_ppuscroll_x,
    output [7:0] o_debug_ppuscroll_y,
    output [15:0] o_debug_ppuaddr,
    output [7:0] o_debug_oamaddr,
    output [14:0] o_debug_v,            // current vram address
    output [14:0] o_debug_t,            // temporary vram address
    output [2:0] o_debug_x,             // fine x scroll
    output o_debug_w,                   // write register (for ppuscroll and ppuaddr)
    output [7:0] o_debug_video_buffer,  // internal buffer of last read from video bus
    output [2:0] o_debug_rasterizer_counter,
    output [31:0] o_debug_palette_0,
    output [31:0] o_debug_palette_1,
    output [31:0] o_debug_palette_2,
    output [31:0] o_debug_palette_3,
    output [31:0] o_debug_palette_4,
    output [31:0] o_debug_palette_5,
    output [31:0] o_debug_palette_6,
    output [31:0] o_debug_palette_7,
    output [5:0] o_debug_colour_index
);

// Screen Constants
localparam [8:0] SCREEN_WIDTH = 341;
localparam [8:0] SCREEN_HEIGHT = 262;

// RS - register select options
localparam [2:0] RS_PPUCTRL = 0;
localparam [2:0] RS_PPUMASK = 1;
localparam [2:0] RS_PPUSTATUS = 2;
localparam [2:0] RS_OAMADDR = 3;
localparam [2:0] RS_OAMDATA = 4;
localparam [2:0] RS_PPUSCROLL = 5;
localparam [2:0] RS_PPUADDR = 6;
localparam [2:0] RS_PPUDATA = 7;

// RW - read / write options
localparam RW_READ = 1;
localparam RW_WRITE = 0;

reg r_int_n;
reg r_video_rd_n;
reg r_video_we_n;
reg r_video_io_is_active;

reg [7:0] r_data;
reg [7:0] r_ppuctrl;
reg [7:0] r_ppumask;
reg [6:0] r_ppustatus;          // note: bit 7 is provided by r_nmi_occurred
reg [15:0] r_ppuaddr;

reg [8:0] r_video_x;
reg [8:0] r_video_y;
wire w_video_visible;

wire w_sprite_zero_hit;

// Palette entries for sprites + background
reg [7:0] r_palette [31:0];

// OAM sprite data
// - 64 sprites
// - 8 bytes per sprite
reg [7:0] r_oamaddr;
reg [7:0] r_oam [255:0];

// lo / hi byte
reg r_w;


// NMI_Occurred
// - set true when vblank starts
// - set false when vblank ends
// - read through ppustatus[7], and set false after the read
reg r_nmi_occurred;

wire [7:0] w_ppustatus = { r_nmi_occurred, r_ppustatus[6:0] };

// NMI_Output
// - PPU pulls o_nmi_n low when nmi_occurred && nmi_output
wire w_nmi_output;
assign w_nmi_output = r_ppuctrl[7];

// internal buffer for last read from video bus
reg [7:0] r_video_buffer;

// address output on video address bus
reg [13:0] r_video_address;

// rendering control signals
wire w_is_rendering_background_enabled = r_ppumask[3];
wire w_is_rendering_sprites_enabled = r_ppumask[4];
wire w_is_rendering_enabled = w_is_rendering_background_enabled | w_is_rendering_sprites_enabled;

//
// READ - PPU/OAM Registers
//

always @(*)
begin
    r_data = 0;

    if (i_cs_n == 0)
    begin
        if (i_rw == RW_READ)
        begin
            case (i_rs)
            RS_PPUSTATUS: begin
                r_data = w_ppustatus;
            end
            RS_PPUDATA: begin
                if ((r_ppuaddr >= 16'h3F00) && (r_ppuaddr <= 16'h3FFF)) 
                begin
                    r_data = r_palette[r_ppuaddr[4:0]];
                end
                else
                begin
                    // the last value received from the VIDEO bus read circuit
                    r_data = r_video_buffer;
                end
            end
            RS_OAMDATA: begin
                r_data = r_oam[r_oamaddr];
            end
            default: begin
                r_data = 0;
            end
            endcase
        end
    end
end

//
// WRITE - PPU/OAM Registers


// + Update T / V registers for rasterizer
// + Rasterizer Counter


always @(negedge i_reset_n or negedge i_clk)
begin
    if (i_reset_n == 0)
    begin
        r_ppuctrl <= 0;
        r_ppumask <= 0;
        r_ppustatus <= 0;
        r_oamaddr <= 0;
    end
    else if (i_ce) 
    begin
        if ((i_cs_n == 0) && (i_rw == RW_WRITE))
        begin
            case (i_rs)
            RS_PPUCTRL: begin
                r_ppuctrl <= i_data;
            end
            RS_PPUMASK: begin
                r_ppumask <= i_data;
            end
            RS_PPUDATA: begin
                if ((r_ppuaddr >= 16'h3F00) && (r_ppuaddr <= 16'h3FFF)) 
                begin
                    if (r_ppuaddr[1:0] == 0)
                    begin
                        // sprite background colour entry is mirrored to background
                        r_palette[r_ppuaddr[4:0] & ~5'h10] <= i_data;               // background
                        r_palette[r_ppuaddr[4:0] | 5'h10] <= i_data;                // sprite
                    end
                    else
                    begin
                        r_palette[r_ppuaddr[4:0]] <= i_data;
                    end
                end
            end
            RS_OAMADDR: begin
                r_oamaddr <= i_data;
            end
            RS_OAMDATA: begin
                r_oamaddr <= r_oamaddr + 1;
                r_oam[r_oamaddr] <= i_data;
            end
            default: begin
            end
            endcase
        end

        // sprite zero hit
        if ((r_video_x == 1) && (r_video_y == (SCREEN_HEIGHT-1))) 
            r_ppustatus[6] <= 0;
        else if (w_is_rendering_enabled && w_is_rendering_background_enabled && w_is_rendering_sprites_enabled)
            r_ppustatus[6] <= r_ppustatus[6] | w_sprite_zero_hit;
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
    else if (i_ce)
    begin
        if ((i_cs_n == 0) && (i_rw == RW_READ) && (i_rs == RS_PPUSTATUS))
        begin
            r_nmi_occurred <= 0;
        end
        else if ((r_video_x == 0) && (r_video_y == 242))
        begin
            // start of vblank
            r_nmi_occurred <= 1;
        end
        else if ((r_video_x == 0) && (r_video_y == (SCREEN_HEIGHT-1)))
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
    else if (i_ce)
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

assign w_video_visible = (r_video_x > 0) && (r_video_x < 256) && (r_video_y < 240);

//
// ppuaddr
//

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_ppuaddr <= 0;
    end
    else if (i_ce)
    begin
        if (i_cs_n == 0)
        begin
            if ((i_rw == RW_WRITE) && (i_rs == RS_PPUADDR))
            begin
                if (r_w == 0)
                begin
                    r_ppuaddr[15:8] <= i_data;
                end
                else
                begin
                    r_ppuaddr[7:0] <= i_data;
                end
            end
            else if (i_rs == RS_PPUDATA)
            begin
                r_ppuaddr <= r_ppuaddr + ( r_ppuctrl[2] ? 32 : 1 );
            end
        end
    end
end

//
// w
//

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_w <= 0;
    end
    else if (i_ce)
    begin
        if (i_cs_n == 0)
        begin
            if ((i_rw == RW_READ) && (i_rs == RS_PPUSTATUS))
            begin
                r_w <= 0;
            end
            else if (i_rw == RW_WRITE)
            begin
                case (i_rs)
                RS_PPUSCROLL, RS_PPUADDR:
                begin
                    r_w <= !r_w;
                end
                default:
                begin
                end
                endcase
                
            end
        end
    end
end

//
// VRAM read/write
//
// note: take two cycles for each read/write
//
always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_video_rd_n <= 1;
        r_video_we_n <= 1;
        r_video_buffer <= 0;
        r_video_io_is_active <= 0;
    end
    else if (i_ce)
    begin
        if ((i_cs_n == 0) && (i_rs == RS_PPUDATA))
        begin
            if (i_rw == RW_WRITE)
            begin
                if (r_ppuaddr < 16'h3f00)
                begin
                    // WRITE ppudata to video bus
                    r_video_we_n <= 0;
                    r_video_buffer <= i_data;

                    r_video_io_is_active <= 1;
                end
            end
            else
            begin
                // READ ppudata from video bus into r_video_buffer
                r_video_rd_n <= 0;
                r_video_buffer <= i_data;

                r_video_io_is_active <= 1;
            end

            r_video_address <= r_ppuaddr[13:0];
        end
        else if (r_video_io_is_active == 1)
        begin
            if (!r_video_we_n)
            begin
                r_video_we_n <= 1;
            end
            else if (!r_video_rd_n)
            begin
                r_video_rd_n <= 1;
                r_video_buffer <= i_vram_data;
            end

            r_video_io_is_active <= 0;
        end
    end
end

//
// Video output
// 

// background rendering
wire [13:0] w_background_video_address;
wire w_background_video_rd_n;
wire [3:0] w_background_palette_index;
wire [2:0] w_debug_rasterizer_counter;
wire [14:0] w_debug_v;
wire [14:0] w_debug_t;
wire [2:0] w_debug_x;

Background background(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_cs_n(i_cs_n),
    .i_ce(i_ce),
    
    .i_is_rendering_enabled(w_is_rendering_enabled),
    .i_is_rendering_background_enabled(w_is_rendering_background_enabled),
    .i_video_visible(w_video_visible),
    .i_video_x( (r_video_x+1) % SCREEN_WIDTH),              // Offset Background by 1 pixel
                                                            // to fix left edge, and to align with sprites
    .i_video_y(r_video_y),
    .i_ppuctrl_b(r_ppuctrl[4]),
    .i_w(r_w),

    .i_rs(i_rs),
    .i_data(i_data),
    .i_rw(i_rw),

    .i_vram_data(i_vram_data),
    .o_video_address(w_background_video_address),
    .o_video_rd_n(w_background_video_rd_n),
    
    .o_palette_index(w_background_palette_index),

    .o_debug_rasterizer_counter(w_debug_rasterizer_counter),
    .o_debug_v(w_debug_v),
    .o_debug_t(w_debug_t),
    .o_debug_x(w_debug_x)
);


// foreground rendering

wire [3:0] w_sprites_palette_index;
wire [13:0] w_sprites_video_address;
wire w_sprites_video_rd_n;
wire w_sprite_priority;
wire w_sprite_zero;

Sprites sprites(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_ce(i_ce),

    .i_is_rendering_enabled(w_is_rendering_enabled),
    .i_is_rendering_sprites_enabled(w_is_rendering_sprites_enabled),
    .i_oam(r_oam),
    .i_video_x(r_video_x),
    .i_video_y(r_video_y),
    .i_ppuctrl_s(r_ppuctrl[3]),

    .i_vram_data(i_vram_data),
    .o_video_address(w_sprites_video_address),
    .o_video_rd_n(w_sprites_video_rd_n),
    .o_palette_index(w_sprites_palette_index),
    .o_priority(w_sprite_priority),
    .o_sprite_zero(w_sprite_zero)
);

// detect sprite zero hit
assign w_sprite_zero_hit = w_sprite_zero && (w_sprites_palette_index != 0) && (w_background_palette_index != 0);

// convert background and sprite layers into an output colour

reg [4:0] r_palette_index;
reg [5:0] r_colour_index;

always @(*)
begin
    r_palette_index = ( ((w_sprites_palette_index != 0) && w_sprite_priority) || (w_background_palette_index == 0))
                        ? {1'b1, w_sprites_palette_index}
                        : {1'b0, w_background_palette_index};

    r_colour_index = r_palette[r_palette_index][5:0];
end

// conversion from colour index to RGB video output
PaletteLookupRGB palette(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_index(r_colour_index),
    .o_red(o_video_red),
    .o_green(o_video_green),
    .o_blue(o_video_blue)
);

//
// drive outputs
//

assign o_int_n = r_int_n;
assign o_data = r_data;

assign o_vram_rd_n = r_video_rd_n & w_background_video_rd_n & w_sprites_video_rd_n;
assign o_vram_we_n = r_video_we_n;
assign o_video_x = r_video_x;
assign o_video_y = r_video_y;
assign o_video_visible = w_video_visible;

assign o_vram_address = (w_background_video_rd_n == 0) ? w_background_video_address : 
                        (w_sprites_video_rd_n == 0) ? w_sprites_video_address :
                        r_video_address;
assign o_vram_data = (o_vram_we_n == 0) ? r_video_buffer : 0;

assign o_debug_ppuctrl = r_ppuctrl;
assign o_debug_ppumask = r_ppumask;
assign o_debug_ppustatus = w_ppustatus;
assign o_debug_ppuscroll_x = {w_debug_t[4:0], w_debug_x[2:0]};
assign o_debug_ppuscroll_y = {w_debug_t[9:5], w_debug_t[14:12]};
assign o_debug_ppuaddr = r_ppuaddr;
assign o_debug_oamaddr = r_oamaddr;
assign o_debug_v = w_debug_v;
assign o_debug_t = w_debug_t;
assign o_debug_x = w_debug_x;
assign o_debug_w = r_w;
assign o_debug_video_buffer = r_video_buffer;
assign o_debug_rasterizer_counter = w_debug_rasterizer_counter;
assign o_debug_palette_0 = {r_palette[3],r_palette[2],r_palette[1],r_palette[0]};
assign o_debug_palette_1 = {r_palette[7],r_palette[6],r_palette[5],r_palette[4]};
assign o_debug_palette_2 = {r_palette[11],r_palette[10],r_palette[9],r_palette[8]};
assign o_debug_palette_3 = {r_palette[15],r_palette[14],r_palette[13],r_palette[12]};
assign o_debug_palette_4 = {r_palette[19],r_palette[18],r_palette[17],r_palette[16]};
assign o_debug_palette_5 = {r_palette[23],r_palette[22],r_palette[21],r_palette[20]};
assign o_debug_palette_6 = {r_palette[27],r_palette[26],r_palette[25],r_palette[24]};
assign o_debug_palette_7 = {r_palette[31],r_palette[30],r_palette[29],r_palette[28]};
assign o_debug_colour_index = r_colour_index;

endmodule
