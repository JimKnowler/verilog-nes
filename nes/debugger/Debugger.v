/*
 * Debugger
 *
 * Top module for controlling 
 */


module Debugger(
    input           i_clk,
    input           i_reset_n,

    // receive data
    input           i_rx_dv,            // receive data valid
    input [7:0]     i_rx_byte,          // receive data

    // transmit data
    output          o_tx_dv,            // transmit data valid
    output [7:0]    o_tx_byte,          // transmit data

    // current state
    output [7:0]    o_cmd,              // current command
    output [15:0]   o_cmd_bytes_remaining     // number of bytes left for current command
);

localparam [7:0] CMD_NOP = 0;
localparam [7:0] CMD_ECHO = 1;

reg [7:0] r_cmd;                        // current command
reg [15:0] r_cmd_num_bytes;             // number of bytes left for current command

reg r_tx_dv;
reg [7:0] r_tx_byte;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_cmd <= CMD_NOP;
        r_cmd_num_bytes <= 0;
        
        r_tx_dv <= 0;
        r_tx_byte <= 0;
    end
    else
    begin
        if (i_rx_dv)
        begin
            if (r_cmd_num_bytes == 0)
            begin
                // start new command
                r_cmd <= i_rx_byte;

                case (i_rx_byte)
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

                case (r_cmd)
                CMD_ECHO: begin
                    case (r_cmd_num_bytes)
                    2: r_tx_byte <= i_rx_byte;
                    1: r_tx_dv <= 1;
                    default: begin
                    end
                    endcase
                end
                default: begin
                    
                end
                endcase
            end
        end 
        else
        begin
            r_tx_dv <= 0;

            if (r_cmd_num_bytes == 0)
            begin
                r_cmd <= CMD_NOP;
            end
        end
    end
end

assign o_cmd = r_cmd;
assign o_cmd_bytes_remaining = r_cmd_num_bytes;

assign o_tx_dv = r_tx_dv;
assign o_tx_byte = (r_tx_dv) ? r_tx_byte : 8'd0;

endmodule