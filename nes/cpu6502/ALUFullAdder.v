module ALUFullAdder(
    input i_a,
    input i_b,
    input i_carry_in,
    output o_sum,
    output o_carry_out
);

reg r_sum;
reg r_carry_out;

always @(*) begin
    r_sum = (i_a ^ i_b) ^ i_carry_in;
    r_carry_out = (i_a & i_b) | (i_carry_in & (i_a ^ i_b));
end

assign o_sum = r_sum;
assign o_carry_out = r_carry_out;

endmodule
