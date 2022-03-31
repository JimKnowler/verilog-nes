module Background(
    input i_clk,
    input i_reset_n,

    // chip select
    input i_cs_n,

    // clock enable
    input i_ce,    

    // PPU internal signals
    input i_is_rendering_enabled,
    input i_is_rendering_background_enabled,
    input i_video_visible,
    input [8:0] i_video_x,
    input [8:0] i_video_y,
    input i_ppuctrl_b,
    input i_w,

    // CPU interface
    input [2:0] i_rs,
    input [7:0] i_data,    
    input i_rw,

    // VRAM reads
    input [7:0] i_vram_data,
    output [13:0] o_video_address,
    output o_video_rd_n,

    output [3:0] o_palette_index,

    output [2:0] o_debug_rasterizer_counter,
    output [14:0] o_debug_v,
    output [14:0] o_debug_t,
    output [2:0] o_debug_x
);

// RS - register select options
localparam [2:0] RS_PPUCTRL = 0;
localparam [2:0] RS_PPUSCROLL = 5;
localparam [2:0] RS_PPUADDR = 6;
localparam [2:0] RS_PPUDATA = 7;

// RW - read / write options
localparam RW_WRITE = 0;

// background registers
reg [14:0] r_v;
reg [14:0] r_t;
reg [2:0] r_x;

wire w_attribute_table_offset_x;
assign w_attribute_table_offset_x = r_v[1];     // coarse_x[1]

wire w_attribute_table_offset_y;
assign w_attribute_table_offset_y = r_v[6];     // coarse_y[1]

// rasterizer 
reg [2:0] r_rasterizer_counter;
reg r_video_rd_n;
reg [13:0] r_video_address;

// increment x component of v
wire [14:0] w_v_increment_x;
PPUIncrementX ppuIncrementX(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_v(r_v),
    .o_v(w_v_increment_x)
);

// increment y component of v
wire [14:0] w_v_increment_y;
PPUIncrementY ppuIncrementY(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_v(r_v),
    .o_v(w_v_increment_y)
);

// background nametable
reg [7:0] r_video_background_tile;
wire [13:0] w_address_background_tile;
PPUTileAddress ppuTileAddress(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_v(r_v[13:0]),
    .o_address(w_address_background_tile)
);

// background attribute table
reg [1:0] r_video_background_attribute_next;
reg [1:0] r_video_background_attribute;

wire [13:0] w_address_background_attribute;
PPUAttributeAddress ppuAttributeAddressBackground(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_v(r_v[13:0]),
    .o_address(w_address_background_attribute)
);

// background pattern table
reg [7:0] r_video_background_pattern_low;
wire [13:0] w_address_background_patterntable_low;
PPUPatternTableAddress ppuPatternTableAddressBackgroundLow(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_t(r_v[14:12]),                                   // fine Y offset, the row number within a tile
    .i_p(0),                                            // bit plane: 0=lower, 1=upper
    .i_c(r_video_background_tile[3:0]),                 // tile column (=low nibble of nametable entry)
    .i_r(r_video_background_tile[7:4]),                 // tile row (=high nibble of nametable entry)
    .i_h(i_ppuctrl_b),                                  // Half of the sprite table: 0=left, 1=right (from PPUCTRL)
    .o_address(w_address_background_patterntable_low)
);

reg [7:0] r_video_background_pattern_high;
wire [13:0] w_address_background_patterntable_high;
PPUPatternTableAddress ppuPatternTableAddressBackgroundHigh(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_t(r_v[14:12]),                                   // fine Y offset, the row number within a tile
    .i_p(1),                                            // bit plane: 0=lower, 1=upper
    .i_c(r_video_background_tile[3:0]),                 // tile column (=low nibble of nametable entry)
    .i_r(r_video_background_tile[7:4]),                 // tile row (=high nibble of nametable entry)
    .i_h(i_ppuctrl_b),                                  // Half of the sprite table: 0=left, 1=right (from PPUCTRL)
    .o_address(w_address_background_patterntable_high)
);

// background shift registers

/* verilator lint_off UNUSED */
wire [15:0] o_debug_background_data_high;
wire [15:0] o_debug_background_data_low;
wire [7:0] o_debug_background_attribute_table_high;
wire [7:0] o_debug_background_attribute_table_low;
/* verilator lint_on UNUSED */

wire [1:0] w_background_pattern_table;

wire w_is_rasterizer_active;
assign w_is_rasterizer_active = ((i_video_x > 0) && (i_video_x <= 336)) || ((i_video_x >= 321) && (i_video_x <= 336));

wire w_shift_background_shift_registers;
assign w_shift_background_shift_registers = w_is_rasterizer_active;

wire w_load_background_shift_registers;
assign w_load_background_shift_registers = (r_rasterizer_counter == 0) && w_is_rasterizer_active;

Shift16 backgroundShiftPatternTableHigh(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_ce(i_ce),
    .i_load(w_load_background_shift_registers),
    .i_data(r_video_background_pattern_high),
    .i_shift(w_shift_background_shift_registers),
    .i_offset({1'b0,r_x}),
    .o_shift_data(w_background_pattern_table[1]),
    .o_debug_data(o_debug_background_data_high)
);

Shift16 backgroundShiftPatternTableLow(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_ce(i_ce),
    .i_load(w_load_background_shift_registers),
    .i_data(r_video_background_pattern_low),
    .i_shift(w_shift_background_shift_registers),
    .i_offset({1'b0,r_x}),
    .o_shift_data(w_background_pattern_table[0]),
    .o_debug_data(o_debug_background_data_low)
);

wire [1:0] w_video_background_attribute_table;

Shift8 backgroundShiftAttributeTableHigh(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_ce(i_ce),
    .i_load(w_is_rasterizer_active),
    .i_data(r_video_background_attribute[1]),
    .i_shift(w_shift_background_shift_registers),
    .i_offset(r_x),
    .o_shift_data(w_video_background_attribute_table[1]),
    .o_debug_data(o_debug_background_attribute_table_high)
);

Shift8 backgroundShiftAttributeTableLow(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_ce(i_ce),
    .i_load(w_is_rasterizer_active),
    .i_data(r_video_background_attribute[0]),
    .i_shift(w_shift_background_shift_registers),
    .i_offset(r_x),
    .o_shift_data(w_video_background_attribute_table[0]),
    .o_debug_data(o_debug_background_attribute_table_low)
);

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
        r_video_address <= 0;

        r_video_background_attribute <= 0;
        r_video_background_attribute_next <= 0;
        r_video_background_tile <= 0;
        r_video_background_pattern_low <= 0;
        r_video_background_pattern_high <= 0;
    end
    else if (i_ce)
    begin
        if ((i_cs_n == 0) && (i_rs == RS_PPUDATA))
        begin
            r_video_rd_n <= 1;
        end
        else if (i_is_rendering_background_enabled && (
                    ((i_video_y == 261) && (i_video_x >=321)) ||            // pre-render scanline
                    ((i_video_y < 240) && (i_video_x >= 1) && ((i_video_x <= 256 ) || (i_video_x >= 321) ))
                ))
        begin        
            // background render read
            case (r_rasterizer_counter)
            0: begin
                //  0,1 => nametable
                r_video_address <= w_address_background_tile;
                r_video_rd_n <= 0;

                r_video_background_attribute <= r_video_background_attribute_next;
            end
            1: begin
                r_video_background_tile <= i_vram_data;         
                r_video_rd_n <= 1;
            end 
            2: begin
                //  2,3 => attribute table    
                r_video_address <= w_address_background_attribute;
                r_video_rd_n <= 0;   
            end
            3: begin
                // load the background attribute table with the correct 2bits for the current tile position
                if (w_attribute_table_offset_x) 
                begin
                    if (w_attribute_table_offset_y)
                        r_video_background_attribute_next <= i_vram_data[7:6];
                    else
                        r_video_background_attribute_next <= i_vram_data[3:2];
                end
                else
                begin
                    if (w_attribute_table_offset_y)
                        r_video_background_attribute_next <= i_vram_data[5:4];
                    else
                        r_video_background_attribute_next <= i_vram_data[1:0];
                end


                r_video_rd_n <= 1;
            end
            4: begin
                //  4,5 => pattern low 
                r_video_address <= w_address_background_patterntable_low;
                r_video_rd_n <= 0;   
            end
            5: begin
                r_video_background_pattern_low <= i_vram_data;
                r_video_rd_n <= 1;
            end
            6: begin
                //  6,7 => pattern high 
                r_video_address <= w_address_background_patterntable_high;
                r_video_rd_n <= 0; 
            end
            7: begin
                r_video_background_pattern_high <= i_vram_data;
                r_video_rd_n <= 1;            
            end
            endcase
        end
        else
        begin
            r_video_rd_n <= 1;
        end
    end
end


// WRITE - PPU/OAM registers
// + Update T / V registers for rasterizer
// + Rasterizer Counter
always @(negedge i_reset_n or negedge i_clk)
begin
    if (i_reset_n == 0)
    begin
        r_rasterizer_counter <= 0;
        r_v <= 0;
        r_t <= 0;
        r_x <= 0;
    end
    else if (i_ce) 
    begin
        if ((i_cs_n == 0) && (i_rw == RW_WRITE))
        begin
            case (i_rs)
            RS_PPUCTRL: begin
                r_t[11:10] <= i_data[1:0];
            end
            RS_PPUSCROLL: begin
                if (i_w == 0)
                begin
                    // load coarse x into t
                    r_t[4:0] <= i_data[7:3];
                    // load fine x into x
                    r_x <= i_data[2:0];
                end
                else
                begin                    
                    // load y into t
                    r_t[9:5] <= i_data[7:3];
                    r_t[14:12] <= i_data[2:0];
                end
            end
            RS_PPUADDR: begin
                if (i_w == 0)
                begin
                    r_t[13:8] <= i_data[5:0];
                    r_t[14] <= 0;
                end
                else
                begin
                    r_t[7:0] <= i_data[7:0];

                    // 'v' is made identical to final value of t
                    r_v[7:0] <= i_data[7:0];
                    r_v[14:8] <= r_t[14:8];
                end     
            end
            default: begin
            end
            endcase
        end
        else if (i_is_rendering_enabled)
        begin
            if ((i_video_x > 0) && (i_video_x < 256))
            begin
                r_rasterizer_counter <= r_rasterizer_counter + 1;

                if (r_rasterizer_counter == 3'b111)
                begin
                    // increment horiz position in v every 8th pixel
                    r_v <= w_v_increment_x;
                end
            end
            
            if (i_video_x == 256)
            begin
                // dot 256 - increment v vertical position
                r_v <= w_v_increment_y;
            end
            
            if (i_video_x == 257)
            begin
                // dot 257 - copy all hoizontal position bits from t to v    
                r_v[4:0] <= r_t[4:0];
                r_v[10] <= r_t[10];

                // reset rasterizer counter
                r_rasterizer_counter <= 0;
            end

            if ((i_video_y == 261) && (i_video_x >= 280) && (i_video_x <= 304))
            begin
                // dot 280...304 - of pre-render scanline (261) - copy vertical bits from t to v    
                r_v[9:5] <= r_t[9:5];       // coarse y
                r_v[14:11] <= r_t[14:11];   // fine y
            end

            if ((i_video_x >= 321) && (i_video_x <= 336))
            begin
                r_rasterizer_counter <= r_rasterizer_counter + 1;            

                if (r_rasterizer_counter == 3'b111)
                begin
                    // increment horiz position in v every 8th pixel
                    r_v <= w_v_increment_x;
                end
            end        
        end
    end
end

// combinatorial logic to get output palette index
reg [3:0] r_palette_index;

always @(*) 
begin
    r_palette_index = 0;

    if (i_video_visible && i_is_rendering_background_enabled && (w_background_pattern_table > 0))
    begin
        r_palette_index = {w_video_background_attribute_table, w_background_pattern_table};
    end
end


assign o_video_address = r_video_address;
assign o_video_rd_n = r_video_rd_n;
assign o_palette_index = r_palette_index;
assign o_debug_rasterizer_counter = r_rasterizer_counter;
assign o_debug_v = r_v;
assign o_debug_t = r_t;
assign o_debug_x = r_x;


endmodule
