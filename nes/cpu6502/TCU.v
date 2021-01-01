// Timing Control Unit

/// @todo SYNC output 
/// @note The hanson diagram shows SYNC being high during T1
///       but we know SYNC should be high while fetching opcode
///       and that opcode is fetched during T0 
///       ... does T1 == T0 somehow?

module TCU(
    input i_clk,
    input i_reset_n,
    input [2:0] i_tcu_next,
    output [2:0] o_tcu
);

reg [2:0] r_tcu;
reg r_has_started;

always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_tcu <= 0;
        r_has_started <= 0;
    end
    else
    begin
        if (r_has_started)
            r_tcu <= i_tcu_next;
        else
            r_has_started <= 1;
    end
end

assign o_tcu = r_tcu;

endmodule
