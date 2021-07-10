module PPUIncrementY(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n
    /* verilator lint_on UNUSED */
);

// https://wiki.nesdev.com/w/index.php/PPU_scrolling#Y_increment

/*
if ((v & 0x7000) != 0x7000)        // if fine Y < 7
  v += 0x1000                      // increment fine Y
else
  v &= ~0x7000                     // fine Y = 0
  int y = (v & 0x03E0) >> 5        // let y = coarse Y
  if (y == 29)
    y = 0                          // coarse Y = 0
    v ^= 0x0800                    // switch vertical nametable
  else if (y == 31)
    y = 0                          // coarse Y = 0, nametable not switched
  else
    y += 1                         // increment coarse Y
  v = (v & ~0x03E0) | (y << 5)     // put coarse Y back into v
  */

endmodule
