module PPUIncrementY(
    /* verilator lint_off UNUSED */
    input i_clk,
    input i_reset_n,
    /* verilator lint_on UNUSED */

    input [14:0] i_v,
    output [14:0] o_v
);

// https://wiki.nesdev.com/w/index.php/PPU_scrolling#Y_increment

reg [14:0] r_v;

always @(*)
begin
  if ((i_v & 15'h7000) != 15'h7000)
  begin
    // fine
    r_v = i_v + 15'h1000;
  end
  else
  begin
    // coarse
    r_v = i_v & ~15'h7000;      // fine y = 0

    if (i_v[9:5] == 29)
    begin
      // overflow
      r_v[9:5] = 0;
      r_v[11] = ~r_v[11];
    end
    else
    begin
      r_v[9:5] = r_v[9:5] + 1;
    end
  end
end

assign o_v = r_v;

endmodule
