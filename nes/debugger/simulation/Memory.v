// 64K memory for simulation in Verilator

module Memory(
    input i_clk,
    input i_ena,
    input i_wea,
    input [15:0] i_addr,
    input [7:0] i_data,
    output [7:0] o_data
);

localparam MEMORY_NUM_BYTES = 64 * 1024;

reg [7:0] r_memory [MEMORY_NUM_BYTES-1:0];

reg [7:0] r_data;

always @(posedge i_clk)
begin
    if (i_ena)
    begin
        if (i_wea)
        begin
            // write to memory
            r_memory[i_addr] <= i_data;
            // cache for read
            r_data <= i_data;
        end
        else
        begin
            // read
            r_data <= r_memory[i_addr];
        end
    end
end

assign o_data = r_data;

endmodule
