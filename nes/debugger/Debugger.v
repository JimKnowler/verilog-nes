/*
 * Debugger
 *
 * implementation of Debug Protocol for reading/writing 16bit memory regions + 16bit values
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
    output [15:0]   o_mem_address,      // address of memory
    output          o_mem_rw,           // read=1/write=0
    output          o_mem_en,           // memory enable - 1 when active
    output [7:0]    o_mem_data,         // data writing to memory
    input [7:0]     i_mem_data,         // data read from memory

    // value read/write
    output [15:0]   o_value_id,         // id of value
    output          o_value_rw,         // read=1/write=0
    output          o_value_en,         // value enable - 1 when active
    output [15:0]   o_value_data,       // data written to a value
    input [15:0]    i_value_data,       // data read from a value

    // debug state
    output [7:0]    o_debug_cmd,                    // current command
    output [15:0]   o_debug_cmd_bytes_remaining     // number of bytes left for current command
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
localparam [7:0] CMD_VALUE_WRITE = 4;   // 5 bytes:     CMD,
                                        //              RX (valueId hi)
                                        //              RX (valueId lo)
                                        //              RX (value hi)
                                        //              RX (value lo)
localparam [7:0] CMD_VALUE_READ = 5;    // 5 bytes:     CMD,
                                        //              RX (valueId hi)
                                        //              RX (valueId lo)
                                        //              TX (value hi)
                                        //              TX (value lo)

reg [7:0] r_cmd;                        // current command
reg [15:0] r_cmd_num_bytes_remaining;   // number of bytes left for current command
reg [15:0] r_cmd_byte_index;

reg r_tx_dv;
reg [7:0] r_tx_byte;

reg [15:0] r_mem_address;
reg r_mem_rw;
reg r_mem_en;
reg [7:0] r_mem_data;

reg [15:0] r_value_id;
reg r_value_rw;
reg r_value_en;
reg [15:0] r_value_data;


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

        r_mem_address <= 0;
        r_mem_rw <= RW_READ;
        r_mem_en <= 0;
        r_mem_data <= 0;

        r_value_id <= 0;
        r_value_rw <= RW_READ;
        r_value_en <= 0;
        r_value_data <= 0;

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
                CMD_VALUE_WRITE,
                CMD_VALUE_READ: begin
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
                CMD_VALUE_WRITE: begin
                    case (r_cmd_byte_index)
                    0: r_value_id[15:8] <= i_rx_byte;               // valueID hi
                    1: r_value_id[7:0] <= i_rx_byte;                // valueID hi
                    2: r_value_data[15:8] <= i_rx_byte;             // value hi
                    3: begin
                        r_value_data[7:0] <= i_rx_byte;             // value lo
                        r_value_rw <= RW_WRITE;
                        r_value_en <= 1;
                    end
                    default: begin
                        
                    end
                    endcase
                end
                CMD_VALUE_READ: begin
                    case (r_cmd_byte_index)
                    0: r_value_id[15:8] <= i_rx_byte;               // valueID hi
                    1: begin
                        r_value_id[7:0] <= i_rx_byte;               // valueID lo

                        // setup local read from value
                        r_value_rw <= RW_READ;
                        r_value_en <= 1;
                    end
                    default: begin
                        
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

            r_value_rw <= RW_READ;
            r_value_en <= 0;

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
                CMD_VALUE_READ: begin
                    case (r_cmd_byte_index)
                    2: begin
                        r_value_data <= i_value_data;

                        // setup high byte
                        r_tx_dv <= 1;
                        r_tx_byte <= i_value_data[15:8];
                    end
                    3: begin
                        // setup low byte
                        r_tx_dv <= 1;
                        r_tx_byte <= r_value_data[7:0];
                    end
                    default: begin
                    end
                    endcase
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

assign o_debug_cmd = r_cmd;
assign o_debug_cmd_bytes_remaining = r_cmd_num_bytes_remaining;

assign o_tx_dv = r_tx_dv;
assign o_tx_byte = (r_tx_dv) ? r_tx_byte : 8'd0;

assign o_mem_rw = r_mem_rw;
assign o_mem_en = r_mem_en;
assign o_mem_data = (r_mem_rw == RW_WRITE) ? r_mem_data : 0;
assign o_mem_address = (r_mem_en == 1) ? r_mem_address : 0;

assign o_value_rw = r_value_rw;
assign o_value_en = r_value_en;
assign o_value_data = (r_value_rw == RW_WRITE) ? r_value_data : 0;
assign o_value_id = (r_value_en == 1) ? r_value_id : 0;

endmodule