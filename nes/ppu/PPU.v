// PPU - Picture Processing Unit
// AKA "2C02"

module PPU(
    input i_clk,
    input i_reset_n,

    // chip select
    input i_cs_n,

    // CPU interface
    output o_int_n,                 // ~Interrupt, to drive ~NMI on CPU
    input [2:0] i_rs,               // register select
    input [7:0] i_data,             // Read from CPU data bus
    output [7:0] o_data,            // Write to CPU data bus
    input i_rw,                     // Read/~Write for CPU data bus

    /* verilator lint_off UNUSED */
    /* verilator lint_off UNDRIVEN */

    // VIDEO interface    
    output o_video_rd_n,                // ~Read from VIDEO data bus
    output o_video_we_n,                // ~Write to VIDEO data bus
    output [13:0] o_video_address,      // address for video data bus
    output [7:0] o_video_data,          // data write to video data bus
    input [7:0] i_video_data,           // data read from video data bus

    // Video output
    output [7:0] o_video_red,
    output [7:0] o_video_green,
    output [7:0] o_video_blue,
    output o_video_x,
    output o_video_y,

    /* verilator lint_on UNDRIVEN */
    /* verilator lint_on UNUSED */

    // debug ports
    output [7:0] o_debug_ppuctrl
);

localparam [2:0] RS_PPUCTRL = 0;
localparam [2:0] RS_PPUSTATUS = 2;
localparam RW_READ = 1;
localparam RW_WRITE = 0;

reg r_int_n;
reg r_video_rd_n;
reg r_video_we_n;

reg [7:0] r_data;
reg [7:0] r_ppuctrl;
reg [7:0] r_ppustatus;

/* verilator lint_off UNUSED */

// OAM sprite data
reg [7:0] r_oam [255:0];

// NMI_Occurred
// - set true when vblank starts
// - set false when vblank ends
// - read through ppustatus[7], and set false after the read
reg r_nmi_occurred;

// NMI_Output
// - PPU pulls o_nmi_n low when nmi_occurred && nmi_output
// - set by writes to ppuctrl[7]
reg r_nmi_output;

/* verilator lint_on UNUSED */

always @(*)
begin
    r_int_n = 1;
    r_video_rd_n = 1;
    r_video_we_n = 1;
    
    if (i_rw == RW_READ)
    begin
        case (i_rs)
        RS_PPUSTATUS: begin
            r_data = r_ppustatus;
        end
        default: begin
            r_data = 0;
        end
        endcase
    end
    else
    begin
        r_data = 0;
    end
end

always @(negedge i_reset_n or negedge i_clk)
begin
    if (i_reset_n == 0)
    begin
        r_ppuctrl <= 0;
        r_ppustatus <= 0;
    end
    else if (i_cs_n == 0)
    begin
        if (i_rw == RW_WRITE)
        begin
            case (i_rs)
            RS_PPUCTRL: begin
                r_ppuctrl <= i_data;
            end
            default: begin
            end
            endcase
        end
    end
end

assign o_int_n = r_int_n;
assign o_video_rd_n = r_video_rd_n;
assign o_video_we_n = r_video_we_n;
assign o_data = r_data;
assign o_debug_ppuctrl = r_ppuctrl;

endmodule