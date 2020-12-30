module Decoder(
    input [7:0] i_ir,
    input [2:0] i_tcu,
    output reg [2:0] o_tcu_next,
    output reg o_rw
);

////////////////////////////////////////////
// tcu

always @(i_ir or i_tcu)
begin
    o_tcu_next = i_tcu + 1;
end

////////////////////////////////////////////
// rw

localparam RW_READ = 1'b1;
localparam RW_WRITE = 1'b0;

localparam OPCODE_STAa = 8'h8D;

always @(i_ir or i_tcu)
begin
    o_rw = RW_READ;

    if ((i_ir == OPCODE_STAa) && (i_tcu==4))
    begin
        o_rw = RW_WRITE;
    end
end

endmodule
