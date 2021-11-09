/*
 * Debugger
 *
 * Top module for controlling 
 */


module Debugger(
    input           i_clk,
    input           i_reset_n,

    /* verilator lint_off UNUSED */
    input           i_rx_dv,
    input [7:0]     i_rx,
    /* verilator lint_on UNUSED */

    output [7:0]    o_cmd,              // current command
    output [15:0]   o_cmd_num_bytes     // number of bytes left for current command
);

localparam [7:0] CMD_NOP = 0;
localparam [7:0] CMD_ECHO = 1;

reg [7:0] r_cmd;                        // current command
reg [15:0] r_cmd_num_bytes;             // number of bytes left for current command

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_cmd <= CMD_NOP;
        r_cmd_num_bytes <= 0;
    end
    else
    begin
        if (i_rx_dv)
        begin
            if (r_cmd_num_bytes == 0)
            begin
                // ready for next cmd
                r_cmd <= i_rx;

                case (i_rx)
                CMD_ECHO: begin
                    r_cmd_num_bytes <= 2;
                end
                default: begin
                    r_cmd_num_bytes <= 0;
                end
                endcase
            end
            else
            begin
                r_cmd_num_bytes <= r_cmd_num_bytes - 1;
            end
        end 
        else
        begin
            if (r_cmd_num_bytes == 0)
            begin
                r_cmd <= CMD_NOP;
            end
        end
    end
end

assign o_cmd = r_cmd;
assign o_cmd_num_bytes = r_cmd_num_bytes;

endmodule