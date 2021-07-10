module PPUIncrementX(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n
    /* verilator lint_on UNUSED */
);

//https://wiki.nesdev.com/w/index.php/PPU_scrolling#Coarse_X_increment

/*
if ((v & 0x001F) == 31) // if coarse X == 31
  v &= ~0x001F          // coarse X = 0
  v ^= 0x0400           // switch horizontal nametable
else
  v += 1                // increment coarse X
*/

endmodule
