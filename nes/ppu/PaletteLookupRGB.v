// Palette Lookup - convert from palette index to RGB
// Based on 2C02 palette documented here: https://wiki.nesdev.com/w/index.php/PPU_palettes


module PaletteLookupRGB(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [5:0] i_index,    
    output [7:0] o_red,
    output [7:0] o_green,
    output [7:0] o_blue
);

reg [7:0] r_red;
reg [7:0] r_green;
reg [7:0] r_blue;

always @(*)
begin
    case (i_index)
    // >> generated by generate_palette_case.py
    0: begin
        r_red = 84; r_green = 84; r_blue = 84;
    end
    1: begin
        r_red = 0; r_green = 30; r_blue = 116;
    end
    2: begin
        r_red = 8; r_green = 16; r_blue = 144;
    end
    3: begin
        r_red = 48; r_green = 0; r_blue = 136;
    end
    4: begin
        r_red = 68; r_green = 0; r_blue = 100;
    end
    5: begin
        r_red = 92; r_green = 0; r_blue = 48;
    end
    6: begin
        r_red = 84; r_green = 4; r_blue = 0;
    end
    7: begin
        r_red = 60; r_green = 24; r_blue = 0;
    end
    8: begin
        r_red = 32; r_green = 42; r_blue = 0;
    end
    9: begin
        r_red = 8; r_green = 58; r_blue = 0;
    end
    10: begin
        r_red = 0; r_green = 64; r_blue = 0;
    end
    11: begin
        r_red = 0; r_green = 60; r_blue = 0;
    end
    12: begin
        r_red = 0; r_green = 50; r_blue = 60;
    end
    13: begin
        r_red = 0; r_green = 0; r_blue = 0;
    end
    14: begin
        r_red = 0; r_green = 0; r_blue = 0;
    end
    15: begin
        r_red = 0; r_green = 0; r_blue = 0;
    end
    16: begin
        r_red = 152; r_green = 150; r_blue = 152;
    end
    17: begin
        r_red = 8; r_green = 76; r_blue = 196;
    end
    18: begin
        r_red = 48; r_green = 50; r_blue = 236;
    end
    19: begin
        r_red = 92; r_green = 30; r_blue = 228;
    end
    20: begin
        r_red = 136; r_green = 20; r_blue = 176;
    end
    21: begin
        r_red = 160; r_green = 20; r_blue = 100;
    end
    22: begin
        r_red = 152; r_green = 34; r_blue = 32;
    end
    23: begin
        r_red = 120; r_green = 60; r_blue = 0;
    end
    24: begin
        r_red = 84; r_green = 90; r_blue = 0;
    end
    25: begin
        r_red = 40; r_green = 114; r_blue = 0;
    end
    26: begin
        r_red = 8; r_green = 124; r_blue = 0;
    end
    27: begin
        r_red = 0; r_green = 118; r_blue = 40;
    end
    28: begin
        r_red = 0; r_green = 102; r_blue = 120;
    end
    29: begin
        r_red = 0; r_green = 0; r_blue = 0;
    end
    30: begin
        r_red = 0; r_green = 0; r_blue = 0;
    end
    31: begin
        r_red = 0; r_green = 0; r_blue = 0;
    end
    32: begin
        r_red = 236; r_green = 238; r_blue = 236;
    end
    33: begin
        r_red = 76; r_green = 154; r_blue = 236;
    end
    34: begin
        r_red = 120; r_green = 124; r_blue = 236;
    end
    35: begin
        r_red = 176; r_green = 98; r_blue = 236;
    end
    36: begin
        r_red = 228; r_green = 84; r_blue = 236;
    end
    37: begin
        r_red = 236; r_green = 88; r_blue = 180;
    end
    38: begin
        r_red = 236; r_green = 106; r_blue = 100;
    end
    39: begin
        r_red = 212; r_green = 136; r_blue = 32;
    end
    40: begin
        r_red = 160; r_green = 170; r_blue = 0;
    end
    41: begin
        r_red = 116; r_green = 196; r_blue = 0;
    end
    42: begin
        r_red = 76; r_green = 208; r_blue = 32;
    end
    43: begin
        r_red = 56; r_green = 204; r_blue = 108;
    end
    44: begin
        r_red = 56; r_green = 180; r_blue = 204;
    end
    45: begin
        r_red = 60; r_green = 60; r_blue = 60;
    end
    46: begin
        r_red = 0; r_green = 0; r_blue = 0;
    end
    47: begin
        r_red = 0; r_green = 0; r_blue = 0;
    end
    48: begin
        r_red = 236; r_green = 238; r_blue = 236;
    end
    49: begin
        r_red = 168; r_green = 204; r_blue = 236;
    end
    50: begin
        r_red = 188; r_green = 188; r_blue = 236;
    end
    51: begin
        r_red = 212; r_green = 178; r_blue = 236;
    end
    52: begin
        r_red = 236; r_green = 174; r_blue = 236;
    end
    53: begin
        r_red = 236; r_green = 174; r_blue = 212;
    end
    54: begin
        r_red = 236; r_green = 180; r_blue = 176;
    end
    55: begin
        r_red = 228; r_green = 196; r_blue = 144;
    end
    56: begin
        r_red = 204; r_green = 210; r_blue = 120;
    end
    57: begin
        r_red = 180; r_green = 222; r_blue = 120;
    end
    58: begin
        r_red = 168; r_green = 226; r_blue = 144;
    end
    59: begin
        r_red = 152; r_green = 226; r_blue = 180;
    end
    60: begin
        r_red = 160; r_green = 214; r_blue = 228;
    end
    61: begin
        r_red = 160; r_green = 162; r_blue = 160;
    end
    62: begin
        r_red = 0; r_green = 0; r_blue = 0;
    end
    63: begin
        r_red = 0; r_green = 0; r_blue = 0;
    end
    // << generated by generate_palette_case.py

    endcase
end

assign o_red = r_red;
assign o_green = r_green;
assign o_blue = r_blue;

endmodule
