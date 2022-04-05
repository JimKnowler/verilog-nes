module Sprites(
    input i_clk,
    input i_reset_n,
    input i_ce,

    // PPU internal signals
    input i_is_rendering_enabled,
    input i_is_rendering_sprites_enabled,
    input [7:0] i_oam [255:0],
    input [8:0] i_video_x,
    input [8:0] i_video_y,
    input i_ppuctrl_s,              // Sprite pattern table address for 8x8 sprites (ignored in 8x16 mode)
                                    // 0: $0000, 1: $1000

    // TODO: add i_ppuctrl_h (ppuctrl bit 5) for sprite size (0: 8x8, 1: 8x16)    

    // vram reads
    input [7:0] i_vram_data,
    output [13:0] o_video_address,
    output o_video_rd_n,
    output [3:0] o_palette_index,
    output o_priority,
    output o_sprite_zero
);

// Secondary OAM
// - sprites to render on the current row
// - 8 sprites x 4 bytes
reg [7:0] r_secondary_oam [31:0];

// indexes into the secondary oam
reg [4:0] r_secondary_oam_index;
reg [4:0] r_clear_secondary_oam_index;

// index into the primary oam
reg [7:0] r_primary_oam_index;

// detect whether the current cycle (i.e. x co-ord) is odd or even
wire w_is_odd_cycle;
assign w_is_odd_cycle = (i_video_x & 1) == 1;

// temp buffer for copying oam data
reg [7:0] r_oam_buffer;

// field of OAM that is current focus of sprite evaluation
reg [1:0] r_oam_field;

// Fields within each OAM entry
localparam [1:0] OAM_FIELD_Y_COORD = 0;
//localparam [1:0] OAM_FIELD_TILE_INDEX = 1;
//localparam [1:0] OAM_FIELD_ATTRIBUTES = 2;
localparam [1:0] OAM_FIELD_X_COORD = 3;

// VRAM Read
reg r_video_rd_n;
reg [13:0] r_video_address;

// Sprite Fetching
reg [2:0] r_fetch_sprite_index;
reg [2:0] r_fetch_sprite_cycle; 

reg [7:0] r_fetch_sprite_field_ycoord;
reg [7:0] r_fetch_sprite_field_tile;
reg [7:0] r_fetch_sprite_field_attributes [7:0];
reg [7:0] r_fetch_sprite_field_xcoord [7:0];

reg r_sprite_zero;
reg r_sprite_zero_next_scanline;

// is rasterizer active
reg r_is_rasterizer_active;
assign r_is_rasterizer_active = (i_video_x > 0) && (i_video_x < 256);

// 8x8 Sprite tile address lookup

wire [13:0] w_address_tile_lo;
wire [13:0] w_address_tile_hi;

reg [2:0] r_fine_y_offset;
always @(*)
begin
    r_fine_y_offset = {r_fetch_sprite_field_ycoord - i_video_y - 8'd2}[2:0];

    // vertical flip
    if (r_fetch_sprite_field_attributes[r_fetch_sprite_index][7] == 0)
        r_fine_y_offset = 3'd7 - r_fine_y_offset; 
end

PPUSprite8x8TileAddress ppuSprite8x8TileAddressLo(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_fine_y_offset(r_fine_y_offset),               // TODO: handle vertical flip
    .i_tile(r_fetch_sprite_field_tile),
    .i_bit_plane(0),
    .i_ppuctrl_s(i_ppuctrl_s),
    .o_address(w_address_tile_lo)
);

PPUSprite8x8TileAddress ppuSprite8x8TileAddressHi(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),
    .i_fine_y_offset(r_fine_y_offset),               // TODO: handle vertical flip
    .i_tile(r_fetch_sprite_field_tile),
    .i_bit_plane(1),
    .i_ppuctrl_s(i_ppuctrl_s),
    .o_address(w_address_tile_hi)
);

////////////////////////////////////////////
// Active Sprite rasterizers

// bitfields - each bit corresponds to a sprite renderer

reg [7:0] r_bitfield_tile_shift_load_lo;        // control signal - load data into specified shift registers
reg [7:0] r_bitfield_tile_shift_load_hi;        // control signal - load data into specified shift registers
wire [7:0] w_bitfield_tile_shift_data_lo;       // data output from shift registers
wire [7:0] w_bitfield_tile_shift_data_hi;       // data output from shift registers
reg [7:0] r_bitfield_tile_shift_is_active;     // status signal - report when shift register has valid output data for current cycle

reg [7:0] r_shift_load_data;
always @(*)
begin
    r_shift_load_data = i_vram_data;

    // flip horizontal
    if (r_fetch_sprite_field_attributes[r_fetch_sprite_index][6] == 1)
        r_shift_load_data = {<<{i_vram_data}};      // SystemVerilog stream operator: https://www.amiq.com/consulting/2017/05/29/how-to-pack-data-using-systemverilog-streaming-operators/#reverse_bits
end

genvar i;
generate
    for (i=0; i<8; i=i+1) begin
        /* verilator lint_off UNUSED */
        wire [7:0] w_debug_shift_data_lo;
        wire [7:0] w_debug_shift_data_hi;
        /* verilator lint_on UNUSED */
        
        ShiftParallelLoad8 shiftTileLo(
            .i_clk(i_clk),
            .i_reset_n(i_reset_n),
            .i_ce(i_ce),
            .i_load(r_bitfield_tile_shift_load_lo[i]),
            .i_data(r_shift_load_data),
            .i_shift(r_bitfield_tile_shift_is_active[i]),
            .o_shift_data(w_bitfield_tile_shift_data_lo[i]),
            
            .o_debug_data(w_debug_shift_data_lo)
        );

        ShiftParallelLoad8 shiftTileHi(
            .i_clk(i_clk),
            .i_reset_n(i_reset_n),
            .i_ce(i_ce),
            .i_load(r_bitfield_tile_shift_load_hi[i]),
            .i_data(r_shift_load_data),
            .i_shift(r_bitfield_tile_shift_is_active[i]),
            .o_shift_data(w_bitfield_tile_shift_data_hi[i]),
            
            .o_debug_data(w_debug_shift_data_hi)
        );

        always @(*)
        begin
            r_bitfield_tile_shift_load_lo[i] = (i[2:0] == r_fetch_sprite_index) && (r_fetch_sprite_cycle == 5);
            r_bitfield_tile_shift_load_hi[i] = (i[2:0] == r_fetch_sprite_index) && (r_fetch_sprite_cycle == 7);
            r_bitfield_tile_shift_is_active[i] = (r_fetch_sprite_field_xcoord[i] == 0);
        end
    end
endgenerate

reg r_rasterizer_is_sprite_active;
reg [2:0] r_rasterizer_sprite_index;

SpriteRasterizerPriority spriteRasterizerPriority(
    .i_clk(i_clk),
    .i_reset_n(i_reset_n),

    .i_bitfield_tile_data_valid(r_bitfield_tile_shift_is_active),
    .i_bitfield_tile_data_lo(w_bitfield_tile_shift_data_lo),
    .i_bitfield_tile_data_hi(w_bitfield_tile_shift_data_hi),
    .o_is_sprite_active(r_rasterizer_is_sprite_active),
    .o_sprite_index(r_rasterizer_sprite_index)
);

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_clear_secondary_oam_index <= 0;

        r_primary_oam_index <= 0;
        r_secondary_oam_index <= 0;

        r_sprite_zero <= 0;
        r_sprite_zero_next_scanline <= 0;

        r_video_rd_n <= 1;
        r_video_address <= 0;
    end
    else if (i_ce & i_is_rendering_enabled)
    begin
        ////////////////////////////////////////////
        // cycles 1-64: clear secondary OAM to 0xFF

        if (i_video_x == 0)
        begin
            // reset index used by secondary oam clear
            r_clear_secondary_oam_index <= 0;

            // update r_sprite_zero for this scanline
            r_sprite_zero <= r_sprite_zero_next_scanline;
        end
        else if (i_video_x < 65)
        begin
            if (w_is_odd_cycle)
                r_secondary_oam[ r_clear_secondary_oam_index ] <= 8'hFF;
            else
                r_clear_secondary_oam_index <= (r_clear_secondary_oam_index + 1);
        end

        ////////////////////////////////////////////
        // cycles 65-256: sprite evaluation

        // reset for sprite evaluation 
        if (i_video_x == 64)
        begin
            r_primary_oam_index <= 0;
            r_secondary_oam_index <= 0;
            r_oam_field <= OAM_FIELD_Y_COORD;
        end

        if ((i_video_x >= 65) && (i_video_x <= 256) && (r_secondary_oam_index != 8) && (r_primary_oam_index != 64))
        begin
            if (w_is_odd_cycle)
            begin
                // odd cycle: data ready from primary oam
                r_oam_buffer <= i_oam[(r_primary_oam_index << 2) + {6'b0, r_oam_field}];                
            end 
            else
            begin
                // even cycle: data written to secondary oam
                r_secondary_oam[(r_secondary_oam_index << 2) + {3'b0, r_oam_field}] <= r_oam_buffer;

                r_oam_field <= r_oam_field + 1;

                if (r_primary_oam_index == 0)
                    r_sprite_zero_next_scanline <= 1;

                if (r_oam_field == OAM_FIELD_Y_COORD)
                begin
                    // is y co-ord out of range?
                    if (((i_video_y+1) <= {1'b0, r_oam_buffer}) || ( (i_video_y+1) > ({1'b0, r_oam_buffer} + 8)))
                    begin
                        // skip to next entry in primary OAM
                        r_primary_oam_index <= r_primary_oam_index + 1;
                        // start from Y COORD (first byte in OAM)
                        r_oam_field <= OAM_FIELD_Y_COORD;
                        // sprite zero is not used
                        if (r_primary_oam_index == 0)
                            r_sprite_zero_next_scanline <= 0;
                    end
                end
                
                if (r_oam_field == OAM_FIELD_X_COORD)
                begin
                    // last OAM field, time to mode on to next slots in primary + secondary OAM
                    r_primary_oam_index <= r_primary_oam_index + 1;
                    r_secondary_oam_index <= r_secondary_oam_index + 1;
                end
            end
        end

        ////////////////////////////////////////////
        // cycles 257-320: sprite fetches
        // - 8 sprites
        // - 8 cycles per sprite

        if (i_video_x == 256)
        begin
            // preparation for sprite fetch
            r_fetch_sprite_index <= 0;
            r_fetch_sprite_cycle <= 0;
        end

        if ((i_video_x >= 257) && (i_video_x <= 320))
        begin

            /*
            Cycles 257-320: Sprite fetches (8 sprites total, 8 cycles per sprite)
            1-4: Read the Y-coordinate, tile number, attributes, and X-coordinate of the selected sprite from secondary OAM
            5-8: Read the X-coordinate of the selected sprite from secondary OAM 4 times (while the PPU fetches the sprite tile data)
            For the first empty sprite slot, this will consist of sprite #63's Y-coordinate followed by 3 $FF bytes; for subsequent empty sprite slots, this will be four $FF bytes
            */

            case (r_fetch_sprite_cycle)
            0: r_fetch_sprite_field_ycoord <= r_secondary_oam[(r_fetch_sprite_index << 2) + 0];
            1: r_fetch_sprite_field_tile <= r_secondary_oam[(r_fetch_sprite_index << 2) + 1];
            2: r_fetch_sprite_field_attributes[r_fetch_sprite_index] <= r_secondary_oam[(r_fetch_sprite_index << 2) + 2];
            3: r_fetch_sprite_field_xcoord[r_fetch_sprite_index] <= r_secondary_oam[(r_fetch_sprite_index << 2) + 3];
            4: begin
                // setup tile read layer 0
                r_video_rd_n <= 0;
                r_video_address <= w_address_tile_lo;
            end
            5: begin
                // read tile layer 0
                // (data is now available on i_vram_data until the end of this cycle)
                
                r_video_rd_n <= 1;
            end
            6: begin
                // setup tile read layer 1

                r_video_rd_n <= 0;
                r_video_address <= w_address_tile_hi;
            end
            7: begin
                // read tile layer 1 - data is now available on i_vram_data
                // (data is now available on i_vram_data until the end of this cycle)

                r_video_rd_n <= 1;
            end
            default: begin
            end
            endcase

            r_fetch_sprite_cycle <= r_fetch_sprite_cycle + 1;

            if (r_fetch_sprite_cycle == 7)
                r_fetch_sprite_index <= r_fetch_sprite_index + 1;
        end

        ////////////////////////////////////////////
        // rendering

        //  - while rasterization is active
        //  - select first sprite in secondary oam that is active at current x
        //    - output its' palette index
        //    - output its' priority bit

        if (r_is_rasterizer_active)
        begin

            // decrement xcoord fields for each sprite
            // (we only consider sprites with xcoord == 0 for output)
            if (r_fetch_sprite_field_xcoord[0] != 0)
                r_fetch_sprite_field_xcoord[0] <= r_fetch_sprite_field_xcoord[0] - 1;
            if (r_fetch_sprite_field_xcoord[1] != 0)
                r_fetch_sprite_field_xcoord[1] <= r_fetch_sprite_field_xcoord[1] - 1;
            if (r_fetch_sprite_field_xcoord[2] != 0)
                r_fetch_sprite_field_xcoord[2] <= r_fetch_sprite_field_xcoord[2] - 1;
            if (r_fetch_sprite_field_xcoord[3] != 0)
                r_fetch_sprite_field_xcoord[3] <= r_fetch_sprite_field_xcoord[3] - 1;
            if (r_fetch_sprite_field_xcoord[4] != 0)
                r_fetch_sprite_field_xcoord[4] <= r_fetch_sprite_field_xcoord[4] - 1;
            if (r_fetch_sprite_field_xcoord[5] != 0)
                r_fetch_sprite_field_xcoord[5] <= r_fetch_sprite_field_xcoord[5] - 1;
            if (r_fetch_sprite_field_xcoord[6] != 0)
                r_fetch_sprite_field_xcoord[6] <= r_fetch_sprite_field_xcoord[6] - 1;
            if (r_fetch_sprite_field_xcoord[7] != 0)
                r_fetch_sprite_field_xcoord[7] <= r_fetch_sprite_field_xcoord[7] - 1;
        end
    end
end

// combinatorial logic for output palette index

reg [3:0] r_palette_index;
reg r_priority;

always @(*)
begin
    r_palette_index = 0;
    r_priority = 0;

    if (r_is_rasterizer_active & r_rasterizer_is_sprite_active & i_is_rendering_enabled & i_is_rendering_sprites_enabled)
    begin
        r_palette_index = {
            r_fetch_sprite_field_attributes[r_rasterizer_sprite_index][1:0], 
            w_bitfield_tile_shift_data_hi[r_rasterizer_sprite_index],
            w_bitfield_tile_shift_data_lo[r_rasterizer_sprite_index]
        };

        r_priority = r_fetch_sprite_field_attributes[r_rasterizer_sprite_index][5] == 0;
    end
end

assign o_palette_index = r_palette_index;
assign o_video_rd_n = r_video_rd_n;
assign o_video_address = r_video_address;
assign o_priority = r_priority;
assign o_sprite_zero = r_sprite_zero && (r_rasterizer_sprite_index == 0);

endmodule

