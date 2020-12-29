module Cpu6502(
    input i_clk,
    input i_reset_n
);

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        // todo: reset behaviour
    end
    else
    begin
        // todo: get on with it
    end
end


endmodule
