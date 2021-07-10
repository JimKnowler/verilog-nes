module PPUIncrementX(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [14:0] i_v,
    output [14:0] o_v
);

//https://wiki.nesdev.com/w/index.php/PPU_scrolling#Coarse_X_increment

reg [14:0] r_v;

always @(*)
begin
  if ((i_v & 15'h001F) == 31)
  begin
    // overflow
    r_v = i_v & ~15'h001F;
    r_v ^= 15'h0400;
  end
  else
  begin
    r_v = i_v + 1;
  end
end

assign o_v = r_v;

endmodule
