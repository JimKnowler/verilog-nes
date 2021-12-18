/*
 * Sync - synchronise a 1-bit signal between two clock domains
 */

//`define DEBUG_SYNC

module Sync(
    // clock domain - sending
    input i_clk,
    input i_reset_n,
    input i_data,                // input data
    
    // clock domain - receiving
    input i_sync_clk,            // clk in clock domain receiving signal
    output o_sync_posedge        // detect posedge in data in clock domain receiving signal

`ifdef DEBUG_SYNC
    ,
    output o_debug_data_0,
    output o_debug_data_1,
    output o_debug_data_2
`endif
);

reg r_data_0;
reg r_data_1;
reg r_data_2;

always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_data_0 <= 0;
    end
    else 
    begin
        r_data_0 <= i_data;
    end
end

reg r_sync_posedge;

always @(posedge i_sync_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_data_1 <= 0;
        r_data_2 <= 0;
    end
    else
    begin
        r_data_1 <= r_data_0;
        r_data_2 <= r_data_1;

        r_sync_posedge <= (r_data_1 & !r_data_2);
    end
end

assign o_sync_posedge = r_sync_posedge;

`ifdef DEBUG_SYNC
assign o_debug_data_0 = r_data_0;
assign o_debug_data_1 = r_data_1;
assign o_debug_data_2 = r_data_2;
`endif

endmodule
