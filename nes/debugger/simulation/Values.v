/*
 * Simulation of 16 x 16bit values for debugger to read/write
 */

module Values(
    input i_clk,
    input i_ena,
    input i_wea,
    /* verilator lint_off UNUSED */
    input [15:0] i_id,
    /* verilator lint_on UNUSED */
    input [15:0] i_data,
    output [15:0] o_data
);

localparam NUM_VALUES = 16;

reg [15:0] r_values [NUM_VALUES-1:0];

always @(posedge i_clk)
begin
    if (i_ena)
    begin
        if (i_wea)
        begin
            // write to values
            r_values[i_id[3:0]] <= i_data;
        end
    end
end

assign o_data = i_ena ? r_values[i_id[3:0]] : 0;

endmodule