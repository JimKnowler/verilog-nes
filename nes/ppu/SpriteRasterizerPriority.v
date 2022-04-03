// combinatorial logic

// input 8 * tiles valid
// input 8 * tile values, where {0,0} means sprite is transparent, so not-worthy of consideration

// output: is a sprite active
// output: the selected tile index

module SpriteRasterizerPriority(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [7:0] i_bitfield_tile_data_valid,
    input [7:0] i_bitfield_tile_data_lo,
    input [7:0] i_bitfield_tile_data_hi,

    output o_is_sprite_active,
    output [2:0] o_sprite_index
);
    // index of the selected sprite
    reg [2:0] r_sprite_index;

    // bitfield that reports whether each tile has any active data
    wire [7:0] w_bitfield_tile_has_data;

    genvar i;
    generate
        for (i=0; i<8; i=i+1)
        begin
            assign w_bitfield_tile_has_data[i] = i_bitfield_tile_data_valid[i] & (i_bitfield_tile_data_hi[i] | i_bitfield_tile_data_lo[i]);
        end
    endgenerate

    always @(*)
    begin
        r_sprite_index = 0;

        // priority encoder
        if (w_bitfield_tile_has_data[1]) r_sprite_index = 1;
        else if (w_bitfield_tile_has_data[2]) r_sprite_index = 2;
        else if (w_bitfield_tile_has_data[3]) r_sprite_index = 3;
        else if (w_bitfield_tile_has_data[4]) r_sprite_index = 4;
        else if (w_bitfield_tile_has_data[5]) r_sprite_index = 5;
        else if (w_bitfield_tile_has_data[6]) r_sprite_index = 6;
        else if (w_bitfield_tile_has_data[7]) r_sprite_index = 7;
    end

    // high, if any tile has data
    assign o_is_sprite_active = |w_bitfield_tile_has_data;

    // priority encoded output
    assign o_sprite_index = r_sprite_index;

endmodule


