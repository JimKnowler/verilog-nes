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

    // memory read/write
    output [15:0]   o_mem_address,      // memory address
    output          o_mem_rw,           // read=1/write=0
    output          o_mem_en,           // memory enable - 1 when active
    output [7:0]    o_mem_data,         // data writing to memory

    input [7:0]     i_mem_data,         // data read from memory

    // current state
    output [7:0]    o_cmd,              // current command
    output [15:0]   o_cmd_bytes_remaining     // number of bytes left for current command
);

localparam RW_READ = 1;
localparam RW_WRITE = 0;

localparam [7:0] CMD_NOP = 0;           // 1 BYTE:      CMD
localparam [7:0] CMD_ECHO = 1;          // 3 BYTES:     CMD,
                                        //              RX (value)
                                        //              TX (value)
localparam [7:0] CMD_MEM_WRITE = 2;     // >= 5 BYTES:  CMD, 
                                        //              RX (address hi), 
                                        //              RX (address lo), 
                                        //              RX (num bytes hi), 
                                        //              RX (num bytes lo),
                                        //              RX x (data)
localparam [7:0] CMD_MEM_READ = 3;      // >= 5 BYTES:  CMD, 
                                        //              RX (address hi), 
                                        //              RX (address lo), 
                                        //              RX (num bytes hi), 
                                        //              RX (num bytes lo),
                                        //              TX x n (data)

reg [7:0] r_cmd;                        // current command
reg [15:0] r_cmd_num_bytes_remaining;   // number of bytes left for current command
reg [15:0] r_cmd_byte_index;

reg r_tx_dv;
reg [7:0] r_tx_byte;

reg [15:0] r_mem_address;
reg r_mem_rw;
reg r_mem_en;
reg [7:0] r_mem_data;

reg r_rx_dv_delay_1;
reg r_rx_dv_delay_2;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_cmd <= CMD_NOP;
        r_cmd_num_bytes_remaining <= 0;
        r_cmd_byte_index <= 0;
        
        r_tx_dv <= 0;
        r_tx_byte <= 0;

        r_mem_rw <= RW_READ;
        r_mem_en <= 0;
        r_mem_data <= 0;

        r_rx_dv_delay_1 <= 0;
        r_rx_dv_delay_2 <= 0;
    end
    else
    begin
        r_tx_dv <= 0;
        r_rx_dv_delay_1 <= i_rx_dv;
        r_rx_dv_delay_2 <= r_rx_dv_delay_1;

        if (i_rx_dv)
        begin
            if (r_cmd_num_bytes_remaining == 0)
            begin
                // start new command
                r_cmd <= i_rx_byte;

                r_cmd_byte_index <= 0;

                case (i_rx_byte)
                CMD_ECHO: begin
                    r_cmd_num_bytes_remaining <= 2;
                end
                CMD_MEM_WRITE,
                CMD_MEM_READ: begin
                    // note: this is a temporary length, to be updated when the 
                    //       memory region length is received
                    r_cmd_num_bytes_remaining <= 4;
                end
                default: begin
                    r_cmd_num_bytes_remaining <= 0;
                end
                endcase
            end
            else
            begin
                r_cmd_num_bytes_remaining <= r_cmd_num_bytes_remaining - 1;
                r_cmd_byte_index <= r_cmd_byte_index + 1;
                
                case (r_cmd)
                CMD_ECHO: begin
                    case (r_cmd_byte_index)
                    0: r_tx_byte <= i_rx_byte;
                    default: begin
                    end
                    endcase
                end
                CMD_MEM_WRITE: begin
                    case (r_cmd_byte_index)
                    0: r_mem_address[15:8] <= i_rx_byte;            // address hi
                    1: r_mem_address[7:0] <= i_rx_byte;             // address lo
                    2: r_cmd_num_bytes_remaining <= r_cmd_num_bytes_remaining + {i_rx_byte, 8'b0} - 1; // num bytes hi
                    3: r_cmd_num_bytes_remaining <= r_cmd_num_bytes_remaining + {8'b0, i_rx_byte} - 1;        // num bytes lo
                    default: begin
                        r_mem_rw <= RW_WRITE;
                        r_mem_en <= 1;
                        r_mem_data <= i_rx_byte;
                    end
                    endcase
                end
                CMD_MEM_READ: begin
                    case (r_cmd_byte_index)
                    0: r_mem_address[15:8] <= i_rx_byte;            // address hi
                    1: r_mem_address[7:0] <= i_rx_byte;             // address lo
                    2: r_cmd_num_bytes_remaining <= r_cmd_num_bytes_remaining + {i_rx_byte, 8'b0} - 1; // num bytes hi
                    3: begin
                        r_cmd_num_bytes_remaining <= r_cmd_num_bytes_remaining + {8'b0, i_rx_byte} - 1;        // num bytes lo

                        // start read of first byte from local memory
                        r_mem_en <= 1;
                    end
                    default: begin
                        // start read of next byte from local memory
                        if (r_cmd_num_bytes_remaining > 1)
                            r_mem_en <= 1;
                    end
                    endcase
                end
                default: begin
                    
                end
                endcase
            end
        end 
        else if (r_rx_dv_delay_1)
        begin
            r_mem_rw <= RW_READ;
            r_mem_en <= 0;

            if (r_cmd_num_bytes_remaining == 0)
            begin
                if (r_cmd != CMD_MEM_READ)
                begin
                    r_cmd <= CMD_NOP;
                end
            end
            else 
            begin
                case (r_cmd)
                CMD_ECHO: begin
                    if (r_cmd_byte_index == 1)
                    begin 
                        r_tx_dv <= 1;
                    end
                end
                CMD_MEM_WRITE: begin
                    if (r_cmd_byte_index > 4)
                    begin
                        r_mem_address <= r_mem_address + 1;
                    end
                end
                default: begin
                end
                endcase
            end

        end
        else if (r_rx_dv_delay_2)
        begin
            case (r_cmd)
            CMD_MEM_READ: begin
                if (r_cmd_num_bytes_remaining == 0)
                begin                 
                    r_cmd <= CMD_NOP;
                end
                else
                begin
                    if (r_cmd_byte_index > 3)
                    begin
                        r_tx_byte <= i_mem_data;
                        r_tx_dv <= 1;
                        r_mem_address <= r_mem_address + 1;
                    end
                end
            end
            default: begin
            end
            endcase
        end
    end
end

assign o_cmd = r_cmd;
assign o_cmd_bytes_remaining = r_cmd_num_bytes_remaining;

assign o_tx_dv = r_tx_dv;
assign o_tx_byte = (r_tx_dv) ? r_tx_byte : 8'd0;

assign o_mem_rw = r_mem_rw;
assign o_mem_en = r_mem_en;
assign o_mem_data = (r_mem_rw == RW_WRITE) ? r_mem_data : 0;
assign o_mem_address = (r_mem_en == 1) ? r_mem_address : 0;

endmodule