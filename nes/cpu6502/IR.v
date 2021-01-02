// Instruction Register
// - including Predecode Register + Predecode Logic

module IR(
    input i_clk,
    input i_reset_n,
    input [7:0] i_data,         // input data bus
    input [2:0] i_tcu,          // current microcode step
    output [7:0] o_ir,          // value of instruction register
    input i_interrupt           // force BRK opcode during interrupt
);

reg [7:0] r_pd;
reg [7:0] r_ir;

localparam OPCODE_BRK = 8'h00;

always @(negedge i_clk or posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        // reset
        r_ir <= OPCODE_BRK;
    end
    else if (!i_clk)
    begin
        // phi 2 - clock into predecode register
        r_pd <= i_data;
    end
    else
    begin
        // phi 1 - clock from predecode register into IR
        if (i_tcu == 1)
        begin
            if (i_interrupt)
                r_ir <= OPCODE_BRK;
            else
                r_ir <= r_pd;
        end
    end
end

assign o_ir = r_ir;

endmodule