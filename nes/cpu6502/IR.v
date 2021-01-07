// Instruction Register
// - including Predecode Register + Predecode Logic

module IR(
    input i_clk,
    input i_reset_n,
    input [7:0] i_data,         // input data bus
    input [3:0] i_tcu_next,     // next microcode step
    output [7:0] o_ir           // value of instruction register
);

reg [7:0] r_ir;
reg r_is_interrupt;

// BRK is opcode used during RESET and INTERRUPT
localparam OPCODE_BRK = 8'h00;

always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        // reset
        r_ir <= OPCODE_BRK;
        r_is_interrupt <= 1;
    end
    else
    begin
        // end of phi 2 - clock into IR
        if (i_tcu_next == 1)
        begin
            if (r_is_interrupt)
            begin
                // finished handling this interrupt
                r_is_interrupt <= 0;
            end
            else
            begin
                r_ir <= i_data;
            end
        end
    end
end

assign o_ir = r_ir;

endmodule