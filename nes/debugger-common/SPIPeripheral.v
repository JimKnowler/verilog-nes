/*
 * SPIPeripheral
 *
 * SPI (Serial Peripheral Interface) Peripheral.
 * 
 * Receive a byte from an SPI Controller, while sending a byte
 * response at the same time.
 *
 * Based on https://github.com/nandland/spi-slave 
 *
 * NOTE: FPGA clock speed needs to be 4 x SPI clock frequency.
 *
 * NOTE: when using this module, the output pin connected to 
 *       o_spi_cipo should be disconnected (i.e. 1'bZ) when
 *       chip select (i_spi_cs_n) is inactive (high).
 */
module SPIPeripheral (
    input           i_clk,
    input           i_reset_n,

    // receive data
    output [7:0]    o_rx_byte,      // received data
    output          o_rx_dv,        // data valid - pulse high for 1 cycle when byte received

    // transmit data
    input           i_tx_dv,        // data valid - pulse high for 1 cycle to load tx byte
    input [7:0]     i_tx_byte,      // byte to serialise via CIPO

    // SPI interface
    input           i_spi_clk,      // SPI CLK: clock signal from controller
    output          o_spi_cipo,     // SPI CIPO: tri-state in top module: high-z when cs is positive
    input           i_spi_copi,     // SPI CPOI: only process when cs is negative
    input           i_spi_cs_n,     // chip select (active low)

    // debug internals
    output          o_debug_rx_buffered_2,
    output          o_debug_rx_buffered_1,
    output          o_debug_rx_buffered_0,
    output [2:0]    o_debug_rx_bit_index,
    output [2:0]    o_debug_tx_bit_index,
    output          o_debug_active,
    output [7:0]    o_debug_tx_byte_buffered
);

reg [2:0] r_rx_bit_index;
reg [7:0] r_tx_byte;
reg r_rx_dv;
reg [7:0] r_rx_byte;
reg r_rx_buffered_0;
reg r_rx_buffered_1;
reg r_rx_buffered_2;

reg r_active;

reg [2:0] r_tx_bit_index;  // index into byte being serialised
reg r_tx_cipo;             // current bit being serialised to controller

reg [7:0] r_tx_byte_buffered;
reg r_tx_byte_has_buffered;

// FPGA Clock domain - receive tx_byte when i_tx_dv is pulsed
always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_tx_byte <= 8'h00;
    end
    else
    begin
        if (i_tx_dv) 
        begin
            r_tx_byte <= i_tx_byte;
        end
        else if (r_tx_byte_has_buffered)
        begin
           r_tx_byte <= 0; 
        end
    end
end

// SPI clock domain - send serialised data on CIPO
always @(posedge i_spi_clk or negedge i_reset_n or posedge i_spi_cs_n)
begin
    if ((!i_reset_n) || (i_spi_cs_n == 1'b1))
    begin
        r_active <= 0;
        r_tx_bit_index <= 3'b111;
        r_tx_cipo <= 0;

        r_tx_byte_has_buffered <= 0;
        r_tx_byte_buffered <= 0;
    end
    else
    begin
        r_tx_byte_has_buffered <= 0;

        if (i_spi_cs_n == 1'b0)
        begin
            r_active <= 1;
            r_tx_bit_index <= r_tx_bit_index - 1;

            if (r_tx_bit_index == 3'b111)
            begin
                // first bit, cache the tx byte for transmission
                r_tx_byte_buffered <= r_tx_byte;
                r_tx_byte_has_buffered <= 1;
                r_tx_cipo <= r_tx_byte[r_tx_bit_index];
            end
            else
            begin
                r_tx_cipo <= r_tx_byte_buffered[r_tx_bit_index];
            end
        end
    end
end

// SPI clock domain - read serialised data from COPI
always @(negedge i_spi_clk or negedge i_reset_n or posedge i_spi_cs_n)
begin
    if ((!i_reset_n) || (i_spi_cs_n == 1'b1))
    begin
        r_rx_bit_index <= 3'b111;
        r_rx_byte <= 8'h00;
        r_rx_buffered_0 <= 0;
    end
    else
    begin
        if (i_spi_cs_n == 1'b0)
        begin
            r_rx_bit_index <= r_rx_bit_index - 1;
            r_rx_byte[r_rx_bit_index] <= i_spi_copi;

            if (r_rx_bit_index == 0)
            begin
                // start process of RX crossing to 
                // FPGA clock domain
                r_rx_buffered_0 <= 1;
            end
            else if (r_rx_bit_index == 1)
            begin
                // clear the buffered signal in preparation for next rx
                r_rx_buffered_0 <= 0;
            end
        end
    end
end

// FPGA clock domain - receive RX from spi_clk domain
always @(posedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_rx_dv <= 0;
    end
    else
    begin
        r_rx_buffered_1 <= r_rx_buffered_0;
        r_rx_buffered_2 <= r_rx_buffered_1;

        if ((r_rx_buffered_2 == 1'b0) & (r_rx_buffered_1 == 1'b1))
        begin
            // rising edge
            r_rx_dv <= 1;
        end
        else
        begin
            r_rx_dv <= 0;
        end
    end
end

assign o_rx_byte = o_rx_dv ? r_rx_byte : 0;
assign o_rx_dv = r_rx_dv;
assign o_spi_cipo = r_active ? r_tx_cipo : 0;

// debug ports
assign o_debug_rx_buffered_2 = r_rx_buffered_2;
assign o_debug_rx_buffered_1 = r_rx_buffered_1;
assign o_debug_rx_buffered_0 = r_rx_buffered_0;
assign o_debug_rx_bit_index = r_rx_bit_index;
assign o_debug_tx_bit_index = r_tx_bit_index;
assign o_debug_active = r_active;
assign o_debug_tx_byte_buffered = r_tx_byte_buffered;

endmodule
