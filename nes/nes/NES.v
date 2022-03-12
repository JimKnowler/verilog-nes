module NES(
    input i_clk,                            // 5 MHz
    input i_reset_n,

    //////////////////////////////
    // Video output
    //////////////////////////////

    output [7:0] o_video_red,           // pixel colour - red
    output [7:0] o_video_green,         // pixel colour - green
    output [7:0] o_video_blue,          // pixel colour - blue
    output [8:0] o_video_x,             // pixel clock - x co-ord of current pixel
    output [8:0] o_video_y,             // pixel clock - y co-ord of current pixel
    output o_video_visible,             // is current pixel visible

    //////////////////////////////
    // Controller
    //////////////////////////////

    output o_controller_latch,          // latch (high) signal for controllers
    output o_controller_clk,            // clk (rising edge) signal for controller shift registers
    input i_controller_1,               // input for serial data from controller 1

    //////////////////////////////
    // CPU memory access
    //////////////////////////////

    // RAM (read / write)
    output o_cs_ram,
    output [15:0] o_address_ram,
    output o_rw_ram,
    output [7:0] o_data_ram,
    input [7:0] i_data_ram,

    // PRG (read only)
    output o_cs_prg,
    output [15:0] o_address_prg,
    input [7:0] i_data_prg,

    //////////////////////////////
    // PPU memory access
    //////////////////////////////

    // pattern table
    output o_cs_patterntable,
    input [7:0] i_data_patterntable,
    output o_rw_patterntable,
    output [13:0] o_address_patterntable,

    // nametable
    output o_cs_nametable,
    input [7:0] i_data_nametable,
    output [7:0] o_data_nametable,
    output o_rw_nametable,
    output [13:0] o_address_nametable,

    //////////////////////////////
    // CPU Debugging
    //////////////////////////////

    output [7:0] o_cpu_debug_ir,
    output o_cpu_debug_error,
    output o_cpu_debug_rw,
    output [15:0] o_cpu_debug_address,
    output [3:0] o_cpu_debug_tcu,
    output o_cpu_debug_clk_en,
    output o_cpu_debug_sync,

    //////////////////////////////
    // PPU Debugging
    //////////////////////////////

    output [7:0] o_ppu_debug_ppumask,
    output [7:0] o_ppu_debug_ppuctrl,
    output [7:0] o_ppu_debug_ppustatus,
    output [7:0] o_ppu_debug_ppuscroll_x,
    output [7:0] o_ppu_debug_ppuscroll_y,
    output [13:0] o_ppu_debug_vram_address,
    output [14:0] o_ppu_debug_v,
    output [14:0] o_ppu_debug_t,
    output [2:0] o_ppu_debug_x,
    output o_ppu_debug_w,
    output [2:0] o_ppu_debug_rasterizer_counter,
    output [31:0] o_ppu_debug_palette_0,
    output [31:0] o_ppu_debug_palette_1,
    output [31:0] o_ppu_debug_palette_2,
    output [31:0] o_ppu_debug_palette_3,
    output [31:0] o_ppu_debug_palette_4,
    output [31:0] o_ppu_debug_palette_5,
    output [31:0] o_ppu_debug_palette_6,
    output [31:0] o_ppu_debug_palette_7,
    output [5:0] o_ppu_debug_colour_index
);
    localparam [15:0] ADDRESS_JOY1 = 16'h4016;
    localparam RW_READ = 1;

    wire w_ce_cpu;
    wire w_ce_ppu;

    ClockEnable clockEnable(
        .i_clk(i_clk),
        .i_reset_n(i_reset_n),
        .i_ce(1),                           // todo: expose as module port
        .o_ce_cpu(w_ce_cpu),
        .o_ce_ppu(w_ce_ppu)
    );

    wire w_cpu_rw;
    wire [15:0] w_cpu_address;
    wire [7:0] w_cpu_data_input;
    wire [7:0] w_cpu_data_output;
    wire w_nmi_n;

    wire w_out0;
    wire w_oe1_n;
    
    /* verilator lint_off UNUSED */
    wire w_sync;
    wire [7:0] w_debug_bus_db;
    wire [7:0] w_debug_bus_adl;
    wire [7:0] w_debug_bus_adh;
    wire [7:0] w_debug_bus_sb;
    wire [7:0] w_debug_s;
    wire [7:0] w_debug_pcl;
    wire [7:0] w_debug_pch;
    wire [7:0] w_debug_add;
    wire [7:0] w_debug_dl;
    wire [7:0] w_debug_ac;
    wire [7:0] w_debug_x;
    wire [7:0] w_debug_y;
    wire [7:0] w_debug_p;
    /* verilator lint_on UNUSED */

    Cpu2A03 cpu2A03(
        // 6502
        .i_clk(i_clk),
        .i_reset_n(i_reset_n),
        .i_clk_en(w_ce_cpu),
        .o_rw(w_cpu_rw),
        .o_address(w_cpu_address),
        .i_data(w_cpu_data_input),
        .o_data(w_cpu_data_output),
        .i_irq_n(1),
        .i_nmi_n(w_nmi_n),
        .o_sync(w_sync),

        // controller
        .o_out0(w_out0),
        .o_oe1_n(w_oe1_n),

        // debug
        .o_debug_bus_db(w_debug_bus_db),
        .o_debug_bus_adl(w_debug_bus_adl),
        .o_debug_bus_adh(w_debug_bus_adh),
        .o_debug_bus_sb(w_debug_bus_sb),
        .o_debug_ir(o_cpu_debug_ir),
        .o_debug_tcu(o_cpu_debug_tcu),
        .o_debug_s(w_debug_s),
        .o_debug_pcl(w_debug_pcl),
        .o_debug_pch(w_debug_pch),
        .o_debug_add(w_debug_add),
        .o_debug_dl(w_debug_dl),
        .o_debug_ac(w_debug_ac),
        .o_debug_x(w_debug_x),
        .o_debug_y(w_debug_y),
        .o_debug_p(w_debug_p),
        .o_debug_error(o_cpu_debug_error)
    );

    // is the CPU reading/writing an address that the PPU should respond to
    wire w_cs_ppu;

    PPUChipEnable ppuChipEnable(
        .i_clk(i_clk),
        .i_reset_n(i_reset_n),
        .i_clk_en_cpu(w_ce_cpu),
        .i_clk_en_ppu(w_ce_ppu),
        .i_address(w_cpu_address),
        .o_ce(w_cs_ppu)
    );

    wire w_ppu_vram_we_n;
    wire w_ppu_vram_rd_n;
    wire [13:0] w_ppu_vram_address;
    wire [7:0] w_ppu_vram_data_input;
    wire [7:0] w_ppu_vram_data_output;

    /* verilator lint_off UNUSED */
    wire [15:0] w_debug_ppuaddr;
    wire [7:0] w_debug_oamaddr;
    wire [7:0] w_debug_video_buffer;
    /* verilator lint_on UNUSED */

    wire [2:0] w_rs_ppu;
    wire [7:0] w_data_ppu_cpu;
    wire [7:0] w_data_cpu_ppu;
    wire w_rw_ppu;

    PPU ppu(
        .i_clk(i_clk),
        .i_reset_n(i_reset_n),
        .i_cs_n(~w_cs_ppu),
        .i_ce(w_ce_ppu),
        .o_int_n(w_nmi_n),
        .i_rs(w_rs_ppu),
        .i_data(w_data_cpu_ppu),
        .o_data(w_data_ppu_cpu),
        .i_rw(w_rw_ppu),
        .o_vram_rd_n(w_ppu_vram_rd_n),
        .o_vram_we_n(w_ppu_vram_we_n),
        .o_vram_address(w_ppu_vram_address),
        .o_vram_data(w_ppu_vram_data_output),
        .i_vram_data(w_ppu_vram_data_input),
        .o_video_red(o_video_red),
        .o_video_green(o_video_green),
        .o_video_blue(o_video_blue),
        .o_video_x(o_video_x),
        .o_video_y(o_video_y),
        .o_video_visible(o_video_visible),
        .o_debug_ppuctrl(o_ppu_debug_ppuctrl),
        .o_debug_ppumask(o_ppu_debug_ppumask),
        .o_debug_ppustatus(o_ppu_debug_ppustatus),
        .o_debug_ppuscroll_x(o_ppu_debug_ppuscroll_x),
        .o_debug_ppuscroll_y(o_ppu_debug_ppuscroll_y),
        .o_debug_ppuaddr(w_debug_ppuaddr),
        .o_debug_oamaddr(w_debug_oamaddr),
        .o_debug_v(o_ppu_debug_v),
        .o_debug_t(o_ppu_debug_t),
        .o_debug_x(o_ppu_debug_x),
        .o_debug_w(o_ppu_debug_w),
        .o_debug_video_buffer(w_debug_video_buffer),
        .o_debug_rasterizer_counter(o_ppu_debug_rasterizer_counter),
        .o_debug_palette_0(o_ppu_debug_palette_0),
        .o_debug_palette_1(o_ppu_debug_palette_1),
        .o_debug_palette_2(o_ppu_debug_palette_2),
        .o_debug_palette_3(o_ppu_debug_palette_3),
        .o_debug_palette_4(o_ppu_debug_palette_4),
        .o_debug_palette_5(o_ppu_debug_palette_5),
        .o_debug_palette_6(o_ppu_debug_palette_6),
        .o_debug_palette_7(o_ppu_debug_palette_7),
        .o_debug_colour_index(o_ppu_debug_colour_index)
    );

    CPUMemoryMap cpuMemoryMap(
        // CPU
        .i_clk(i_clk),
        .i_reset_n(i_reset_n),
        .i_clk_en(w_ce_cpu),
        .i_address_cpu(w_cpu_address),
        .i_rw_cpu(w_cpu_rw),
        .i_data_cpu(w_cpu_data_output),
        .o_data_cpu(w_cpu_data_input),

        // RAM (read / write)
        .o_cs_ram(o_cs_ram),
        .o_address_ram(o_address_ram),
        .o_rw_ram(o_rw_ram),
        .o_data_ram(o_data_ram),
        .i_data_ram(i_data_ram),
        // PRG (read only)
        .o_cs_prg(o_cs_prg),
        .o_address_prg(o_address_prg),
        .i_data_prg(i_data_prg),
        // PPU (read / write)
        .o_rs_ppu(w_rs_ppu),
        .o_data_ppu(w_data_cpu_ppu),
        .i_data_ppu(w_data_ppu_cpu),
        .o_rw_ppu(w_rw_ppu),
        // Controller
        .i_oe1_n(w_oe1_n),
        .i_controller_1(i_controller_1)
    );

    PPUMemoryMap ppuMemoryMap(
        .i_clk(i_clk),
        .i_reset_n(i_reset_n),
        .i_address_ppu(w_ppu_vram_address),
        .i_rd_en_ppu_n(w_ppu_vram_rd_n),
        .i_wr_en_ppu_n(w_ppu_vram_we_n),
        .o_data_ppu(w_ppu_vram_data_input),
        .i_data_ppu(w_ppu_vram_data_output),

        // pattern table
        .o_cs_patterntable(o_cs_patterntable),
        .i_data_patterntable(i_data_patterntable),
        .o_rw_patterntable(o_rw_patterntable),
        .o_address_patterntable(o_address_patterntable),

        // nametable
        .o_cs_nametable(o_cs_nametable),
        .i_data_nametable(i_data_nametable),
        .o_data_nametable(o_data_nametable),
        .o_rw_nametable(o_rw_nametable),
        .o_address_nametable(o_address_nametable)
    );

    assign o_cpu_debug_rw = w_cpu_rw;
    assign o_cpu_debug_address = w_cpu_address;
    assign o_cpu_debug_clk_en = w_ce_cpu;
    assign o_ppu_debug_vram_address = w_ppu_vram_address;
    assign o_cpu_debug_sync = w_sync;

    assign o_controller_latch = w_out0;
    assign o_controller_clk = !((w_cpu_rw == RW_READ) && (w_cpu_address == ADDRESS_JOY1));
endmodule