// Model the Decode Rom in 6502
// -> combinatorial logic mapping (IR, TCU, P) => control lines
//

module Decoder(
    input i_reset_n,
    input i_clk,
    input i_clk_en,

    output reg o_error,             // 1 if an error has occurred
    
    input [7:0] i_ir,               // Instruction Register
    input [3:0] i_tcu,              // Opcode timing
    input [7:0] i_p,                // Processor status register
    input i_acr,                    // ALU - carry out
    input i_bus_db_n,               // sign of current value on data bus (JK - cheating)

    input i_irq_n,                  // Interrupt Request
    input i_nmi_n,                  // Non Maskable Interrupt

    output reg [3:0] o_tcu,         // value TCU at next phi2 clock tick

    // control signals
    output reg o_rw,
    output reg o_dl_db,
    output reg o_dl_adl,
    output reg o_dl_adh,
    output reg o_pcl_pcl,
    output reg o_adl_pcl,
    output reg o_i_pc,
    output reg o_pcl_adl,
    output reg o_pcl_db,
    output reg o_pch_pch,
    output reg o_adh_pch,
    output reg o_pch_adh,
    output reg o_pch_db,
    output reg o_x_sb,
    output reg o_y_sb,
    output reg o_ac_sb,
    output reg o_ac_db,
    output reg o_s_sb,
    output reg o_s_adl,
    output reg o_add_sb_7,
    output reg o_add_sb_0_6,
    output reg o_add_adl,
    output reg o_p_db,
    output reg o_0_adl0,
    output reg o_0_adl1,
    output reg o_0_adl2,
    output reg o_0_adh0,
    output reg o_0_adh1_7,
    output reg o_sb_adh,
    output reg o_sb_db,
    output reg o_sb_x,
    output reg o_sb_y,
    output reg o_sb_ac,
    output reg o_sb_s,
    output reg o_adl_abl,
    output reg o_adh_abh,
    output reg o_db_n_add,
    output reg o_db_add,
    output reg o_adl_add,
    output reg o_0_add,
    output reg o_sb_add,
    output reg o_1_addc,
    output reg o_sums,
    output reg o_ands,
    output reg o_eors,
    output reg o_ors,
    output reg o_srs,
    output reg o_dbz_z,
    output reg o_db0_c,
    output reg o_db1_z,
    output reg o_db2_i,
    output reg o_db3_d,
    output reg o_db4_b,
    output reg o_db6_v,
    output reg o_db7_n,
    output reg o_acr_c,
    output reg o_ir5_c,
    output reg o_ir5_i,
    output reg o_ir5_d,
    output reg o_avr_v,

    // JK extra control signals
    output reg o_1_db4
);

// Processor Status Register bitfields
localparam C = 0;       // Carry
localparam Z = 1;       // Zero
localparam I = 2;       // Interrupt Mask
localparam N = 7;       // Negative
localparam V = 6;       // Overflow

// PHI 1 and PHI 2 clock phases
wire w_phi1;
wire w_phi2;
assign w_phi1 = (i_clk == 0);
assign w_phi2 = (i_clk == 1);

// Opcodes
localparam [7:0] BRK = 8'h00,       NOP = 8'hEA,
                 INX = 8'hE8,       INY = 8'hC8,
                 DEX = 8'hCA,       DEY = 8'h88,
                 LDA_i = 8'hA9,     LDA_a = 8'hAD,
                 LDX_i = 8'hA2,     LDX_a = 8'hAE,
                 LDY_i = 8'hA0,     LDY_a = 8'hAC,
                 STA_a = 8'h8D,     BIT_a = 8'h2C,
                 TAX = 8'hAA,       TAY = 8'hA8,
                 TSX = 8'hBA,       TXA = 8'h8A,
                 TXS = 8'h9A,       TYA = 8'h98,
                 LSR_A = 8'h4A,     ASL_A = 8'h0A,
                 CLC = 8'h18,       SEC = 8'h38,
                 SED = 8'hF8,       CLD = 8'hD8,
                 SEI = 8'h78,       CLI = 8'h58,
                 CLV = 8'hb8,
                 ROL_A = 8'h2A,     ROR_A = 8'h6A,
                 PHA = 8'h48,       PHP = 8'h08,
                 PLA = 8'h68,       PLP = 8'h28,
                 AND_i = 8'h29,     EOR_i = 8'h49,
                 ORA_i = 8'h09,     ADC_i = 8'h69,
                 SBC_i = 8'hE9,     CMP_i = 8'hC9,
                 CPX_i = 8'hE0,     CPY_i = 8'hC0,
                 BCC = 8'h90,       BCS = 8'hB0,
                 BEQ = 8'hF0,       BNE = 8'hD0,
                 BMI = 8'h30,       BPL = 8'h10,
                 BVC = 8'h50,       BVS = 8'h70,
                 JMP_a = 8'h4C,     STX_a = 8'h8E,
                 STY_a = 8'h8C,     JSR_a = 8'h20,
                 RTS = 8'h60,       INC_a = 8'hEE,
                 DEC_a = 8'hCE,     RTI = 8'h40,
                 JMP_indirect = 8'h6C,
                 CMP_a = 8'hCD,     CPX_a = 8'hEC,
                 CPY_a = 8'hCC,     ADC_a = 8'h6D,
                 SBC_a = 8'hED,     AND_a = 8'h2D,
                 EOR_a = 8'h4D,     ORA_a = 8'h0D,
                 ASL_a = 8'h0E,     LSR_a = 8'h4E,
                 ROL_a = 8'h2E,     ROR_a = 8'h6E,
                 LDA_ax = 8'hBD,    LDA_ay = 8'hB9,
                 CMP_ax = 8'hDD,    CMP_ay = 8'hD9,
                 LDX_ay = 8'hBE,    LDY_ax = 8'hBC,
                 AND_ax = 8'h3D,    AND_ay = 8'h39,
                 ORA_ax = 8'h1D,    ORA_ay = 8'h19,
                 EOR_ax = 8'h5D,    EOR_ay = 8'h59,
                 STA_ax = 8'h9D,    STA_ay = 8'h99,
                 STA_zp = 8'h85,    STX_zp = 8'h86,     STY_zp = 8'h84,
                 STA_zp_ind_y = 8'h91,
                 LDA_zp_ind_y = 8'hB1,
                 ORA_zp = 8'h05,
                 SBC_ax = 8'hFD, SBC_ay = 8'hF9,
                 INC_zp = 8'hE6, DEC_zp = 8'hC6,
                 EOR_zp = 8'h45, AND_zp = 8'h25,
                 ROR_ax = 8'h7E, ROL_ax = 8'h3E,
                 LDA_zp = 8'hA5, LDX_zp = 8'hA6, LDY_zp = 8'hA4,
                 ADC_zp = 8'h65,
                 ADC_ax = 8'h7D, ADC_ay = 8'h79,
                 BIT_zp = 8'h24, ROL_zp = 8'h26, ROR_zp = 8'h66,
                 LSR_zp = 8'h46, ASL_zp = 8'h06,
                 ASL_ax = 8'h1E, LSR_ax = 8'h5E,
                 DEC_ax = 8'hDE, INC_ax = 8'hFE,
                 CMP_zp = 8'hC5, CPX_zp = 8'hE4, CPY_zp = 8'hC4,
                 LDY_zp_x = 8'hB4, LDA_zp_x = 8'hB5, LDX_zp_y = 8'hB6,
                 STY_zp_x = 8'h94, STA_zp_x = 8'h95, STX_zp_y = 8'h96,
                 ORA_zp_x = 8'h15, AND_zp_x = 8'h35, EOR_zp_x = 8'h55,
                 ADC_zp_x = 8'h75, CMP_zp_x = 8'hD5, SBC_zp_x = 8'hF5,
                 DEC_zp_x = 8'hD6, INC_zp_x = 8'hF6,
                 SBC_zp = 8'hE5,
                 ASL_zp_x = 8'h16, ROL_zp_x = 8'h36,
                 LSR_zp_x = 8'h56, ROR_zp_x = 8'h76,
                 ADC_zp_ind_y = 8'h71, CMP_zp_ind_y = 8'hD1,
                 SBC_zp_ind_y = 8'hF1, 
                 ORA_zp_ind_y = 8'h11, AND_zp_ind_y = 8'h31, EOR_zp_ind_y = 8'h51,
                 ORA_zp_ind_x = 8'h01, AND_zp_ind_x = 8'h21, EOR_zp_ind_x = 8'h41,
                 ADC_zp_ind_x = 8'h61, SBC_zp_ind_x = 8'hE1, CMP_zp_ind_x = 8'hC1,
                 LDA_zp_ind_x = 8'hA1, STA_zp_ind_x = 8'h81;

// RW pin
localparam RW_READ = 1;
localparam RW_WRITE = 0;

// JK - sorry, cheating here until I can figure out a better way 
//      to do this with 6502 internal components
reg r_last_acr;                   // whether carry was set on ALU sum, on the last tick
reg r_bus_db_n;                   // whether value on db was negative at end of last phi2

always @(negedge i_clk or negedge i_reset_n)
begin
    if (!i_reset_n)
    begin
        r_last_acr <= 0;
        r_bus_db_n <= 0;
    end
    else if (i_clk_en == 1)
    begin
        r_last_acr <= i_acr;
        r_bus_db_n <= i_bus_db_n;
    end
end

// distinguish between HW interrupts and BRK opcode interrupt and normal BRK 
// use r_hw_interrupt as local reg storing interrupt type
//    - set to RESET when reset 
//    - set to IRQ while i_irq_n is low
//    - set to NMI when i_nmi_n has falling edge
//    - reset to NONE when executing opcodes (including BRK)

localparam HW_INTERRUPT_NONE = 0;
localparam HW_INTERRUPT_NMI = 1;           // FFFA, FFFB
localparam HW_INTERRUPT_RESET = 2;         // FFFC, FFFD
localparam HW_INTERRUPT_IRQ = 3;           // FFFE, FFFF

reg r_irq_n;                    // buffer IRQ input
reg r_nmi_n;                    // buffer NMI input
reg r_nmi_n_last;               // history of NMI input
reg r_nmi_falling_edge;         // set to 1 when falling edge is detected
reg [1:0] r_hw_interrupt;

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_hw_interrupt <= HW_INTERRUPT_RESET;
        r_irq_n <= 1;
        r_nmi_n <= 1;
        r_nmi_falling_edge <= 0;
    end
    else if (i_clk_en == 1)
    begin
        r_irq_n <= i_irq_n;
        r_nmi_n_last <= r_nmi_n;
        r_nmi_n <= i_nmi_n;

        if (r_nmi_n_last && !r_nmi_n)
        begin
            r_nmi_falling_edge <= 1;
        end
        
        if (r_hw_interrupt != HW_INTERRUPT_NONE)
        begin
            if (i_tcu == 6)
            begin
                r_hw_interrupt <= HW_INTERRUPT_NONE;
            end
        end
        else if (o_tcu == 0)
        begin
            if ((r_nmi_falling_edge == 1))
            begin
                r_hw_interrupt <= HW_INTERRUPT_NMI;
                r_nmi_falling_edge <= 0;
            end
            else if ((r_irq_n == 0) && (i_p[I] == 0))
            begin
                r_hw_interrupt <= HW_INTERRUPT_IRQ;
            end
        end
    end
end

// w_ir => instruction register will usually pass through i_ir, but will report
//         BRK while handling any type of hardware interrupt
wire [7:0] w_ir;
assign w_ir =  (r_hw_interrupt == HW_INTERRUPT_NONE) ? i_ir : BRK;

function void retain_pch(input i_value);
begin
    o_pch_pch = i_value;
end
endfunction

function void retain_pcl(input i_value);
begin
    o_pcl_pcl = i_value;
end
endfunction

function void retain_pc(input i_value);
begin
    retain_pch(i_value);
    retain_pcl(i_value);
end
endfunction

function void output_pch_on_abh(input i_value);
begin
    o_pch_adh = i_value;
    o_adh_abh = i_value;
end
endfunction

function void output_pcl_on_abl(input i_value);
begin
    o_pcl_adl = i_value;
    o_adl_abl = i_value;
end
endfunction

function void output_add_on_abl(input i_value);
begin
    o_add_adl = i_value;
    o_adl_abl = i_value;
end
endfunction

function void output_add_on_sb(input i_value);
begin
    o_add_sb_0_6 = i_value;
    o_add_sb_7 = i_value;
end
endfunction

function void output_add_on_abh(input i_value);
begin
    output_add_on_sb(i_value);
    o_sb_adh = i_value;
    o_adh_abh = i_value;
end
endfunction

function void load_s_from_add(input i_value);
begin
    output_add_on_sb(i_value);    
    o_sb_s = i_value;
end
endfunction

function void load_pcl_from_add(input i_value);
begin
    o_add_adl = i_value;
    o_adl_pcl = i_value;
end
endfunction

function void output_s_on_abl(input i_value);
begin
    o_s_adl = i_value;
    o_adl_abl = i_value;
end
endfunction

function void output_1_on_abh(input i_value);
begin
    o_0_adh1_7 = i_value;
    o_adh_abh = i_value;
end
endfunction

function void output_0_on_abh(input i_value);
begin
    o_0_adh1_7 = i_value;
    o_0_adh0 = i_value;
    o_adh_abh = i_value;
end
endfunction

function void output_dl_on_abl(input i_value);
begin
    o_dl_adl = i_value;
    o_adl_abl = i_value;
end
endfunction

function void load_pch_from_dl(input i_value);
begin
    o_dl_adh = i_value;
    o_adh_pch = i_value;    
end
endfunction

function void output_dl_on_abh(input i_value);
begin
    o_dl_adh = i_value;
    o_adh_abh = i_value;
end
endfunction

function void load_pcl_from_s(input i_value);
begin
    o_s_adl = i_value;
    o_adl_pcl = i_value;
end
endfunction

function void load_p_from_db(input i_value);
begin
    o_db0_c = i_value;
    o_db1_z = i_value;
    o_db2_i = i_value;
    o_db3_d = i_value;
    o_db6_v = i_value;
    o_db7_n = i_value;

    // note: B (break) is ignored
end
endfunction

function void load_add_from_dl(input i_value);
begin
    o_dl_db = i_value;
    o_db_add = i_value;
    o_0_add = i_value;
    o_sums = i_value;
end
endfunction

function void load_abh_with_ff(input i_value);
begin
    // ABH = 0xff (precharge mosfets on adh)
    o_adh_abh = i_value;
end
endfunction

function void increment_pc(input i_value);
begin
    o_i_pc = i_value;
end
endfunction

function void load_add_from_adl(input i_value);
begin
    o_adl_add = i_value;
    o_sums = i_value;
    o_0_add = i_value;
end
endfunction

function void load_add_from_adl_minus_1(input i_value);
begin
    o_adl_add = i_value;
    o_sb_add = i_value;       // pre-charge mosfets = -1
    o_sums = i_value;
end
endfunction

function void load_add_from_adl_plus_1(input i_value);
begin
    load_add_from_adl(i_value);
    o_1_addc = i_value;
end
endfunction

function void next_opcode();
begin
    o_tcu = 0;
end
endfunction

function void load_add_from_sb(input i_value);
begin
    o_sb_add = i_value;
    o_db_n_add = i_value;      // 0 (= inverse of mosfets)
    o_sums = i_value;
end
endfunction

function void load_z_n_from_db(input i_value);
begin
    o_db7_n = i_value;
    o_dbz_z = i_value;
end
endfunction

function void load_s_from_dl(input i_value);
begin
    o_dl_db = i_value;
    o_sb_db = i_value;
    o_sb_s = i_value;
end
endfunction

function void load_ac_from_dl(input i_value);
begin
    o_dl_db = i_value;
    o_sb_db = i_value;
    o_sb_ac = i_value;
end
endfunction

always @(*)
begin
    // default TCU to increment at next clock tick
    o_tcu = i_tcu + 1;

    // default to no error
    o_error = 0;

    // default all control signals
    o_rw = RW_READ;
    o_dl_db = 0;
    o_dl_adl = 0;
    o_dl_adh = 0;
    o_pcl_pcl = 0;
    o_adl_pcl = 0;
    o_i_pc = 0;
    o_pcl_adl = 0;
    o_pcl_db = 0;
    o_pch_pch = 0;
    o_adh_pch = 0;
    o_pch_adh = 0;
    o_pch_db = 0;
    o_x_sb = 0;
    o_y_sb = 0;
    o_ac_sb = 0;
    o_ac_db = 0;
    o_s_sb = 0;
    o_s_adl = 0;
    o_add_sb_7 = 0;
    o_add_sb_0_6 = 0;
    o_add_adl = 0;
    o_p_db = 0;
    o_0_adl0 = 0;
    o_0_adl1 = 0;
    o_0_adl2 = 0;
    o_0_adh0 = 0;
    o_0_adh1_7 = 0;
    o_sb_adh = 0;
    o_sb_db = 0;
    o_sb_x = 0;
    o_sb_y = 0;
    o_sb_ac = 0;
    o_sb_s = 0;
    o_adl_abl = 0;
    o_adh_abh = 0;
    o_db_n_add = 0;
    o_db_add = 0;
    o_adl_add = 0;
    o_0_add = 0;
    o_sb_add = 0;
    o_1_addc = 0;
    o_sums = 0;
    o_ands = 0;
    o_eors = 0;
    o_ors = 0;
    o_srs = 0;
    o_dbz_z = 0;
    o_db0_c = 0;
    o_db1_z = 0;
    o_db2_i = 0;
    o_db3_d = 0;
    o_db4_b = 0;
    o_db6_v = 0;
    o_db7_n = 0;
    o_acr_c = 0;
    o_ir5_c = 0;
    o_ir5_i = 0;
    o_ir5_d = 0;
    o_avr_v = 0;
    o_1_db4 = 0;

    case (i_tcu)
    0:  // T0
    begin        
        // fetch
        output_pcl_on_abl(1);
        output_pch_on_abh(1);
        retain_pc(1);

        if (r_hw_interrupt == HW_INTERRUPT_NONE)
        begin
            increment_pc(1);
        end

        // finish previous opcode
        case (w_ir)
        AND_i, EOR_i, ORA_i, 
        ADC_i, SBC_i,
        CMP_i, CPX_i, CPY_i, CMP_zp_ind_y,
        ORA_zp, ADC_zp,
        ADC_ax, ADC_ay, ADC_zp_ind_y,
        CMP_zp, CPX_zp, CPY_zp,
        ORA_zp_ind_y, AND_zp_ind_y, EOR_zp_ind_y,
        ORA_zp_ind_x, AND_zp_ind_x, EOR_zp_ind_x,
        ADC_zp_ind_x, SBC_zp_ind_x, CMP_zp_ind_x:
        begin
            if (w_phi1)
            begin
                // load immediate operand into ALU via DB from DL
                o_dl_db = 1;

                // load register into ALU via SB
                case (w_ir)
                CPX_i, CPX_zp: o_x_sb = 1;
                CPY_i, CPY_zp: o_y_sb = 1;
                default: o_ac_sb = 1;
                endcase
                    
                o_sb_add = 1;

                case (w_ir)
                AND_i, AND_zp_ind_y, AND_zp_ind_x: begin
                    o_db_add = 1;
                    o_ands = 1;
                end
                EOR_i, EOR_zp_ind_y, EOR_zp_ind_x: begin
                    o_db_add = 1;
                    o_eors = 1;
                end
                ORA_i, ORA_zp, ORA_zp_ind_y, ORA_zp_ind_x: begin
                    o_db_add = 1;
                    o_ors = 1;
                end
                ADC_i, ADC_zp, ADC_ax, ADC_ay, ADC_zp_ind_y, ADC_zp_ind_x: begin
                    o_db_add = 1;
                    o_sums = 1;

                    // carry in
                    o_1_addc = i_p[C];

                    // C + V flags
                    o_acr_c = 1;
                    o_avr_v = 1;
                end
                SBC_i, SBC_zp_ind_x: begin
                    o_db_n_add = 1;
                    o_sums = 1;
                    
                    // carry in
                    o_1_addc = i_p[C];

                    // C + V flags
                    o_acr_c = 1;
                    o_avr_v = 1;
                end
                CMP_i, CPX_i, CPY_i,
                CMP_zp, CPX_zp, CPY_zp,
                CMP_zp_ind_y, CMP_zp_ind_x: begin
                    // subtraction as 2's complement addition
                    o_db_n_add = 1;
                    o_1_addc = 1;

                    o_sums = 1;
                end
                default: begin
                end
                endcase
            end
            else
            begin
                // PHI2
                case (w_ir)
                ADC_i, ADC_zp, ADC_ax, ADC_ay, ADC_zp_ind_y, ADC_zp_ind_x,
                SBC_i, SBC_zp_ind_x,
                CMP_i, CPX_i, CPY_i,
                CMP_zp, CPX_zp, CPY_zp, CMP_zp_ind_y, CMP_zp_ind_x: begin
                    // C flag
                    o_acr_c = 1;
                end
                default: begin
                end
                endcase

                case (w_ir)
                ADC_i, ADC_zp, ADC_ax, ADC_ay, ADC_zp_ind_y, ADC_zp_ind_x,
                SBC_i, SBC_zp_ind_x: begin
                    // V flag
                    o_avr_v = 1;
                end
                default: begin
                end
                endcase

                output_add_on_sb(1);
                
                case (w_ir)
                AND_i, EOR_i, ORA_i, ADC_i, SBC_i, SBC_zp_ind_x,
                ORA_zp, ADC_zp, ADC_ax, ADC_ay, ADC_zp_ind_y, ADC_zp_ind_x,
                ORA_zp_ind_y, AND_zp_ind_y, EOR_zp_ind_y,
                ORA_zp_ind_x, AND_zp_ind_x, EOR_zp_ind_x: begin
                    o_sb_ac = 1;
                end
                default: begin
                end
                endcase
                
                o_sb_db = 1;
                load_z_n_from_db(1);
            end
        end
        LDA_a, LDX_a, LDY_a, LDA_zp_ind_y, LDA_zp, LDX_zp, LDY_zp, 
        LDY_zp_x, LDA_zp_x, LDX_zp_y, LDA_zp_ind_x:
        begin
            if (w_phi1)
            begin
                load_add_from_dl(1);
            end
            else
            begin
                // phi2
                output_add_on_sb(1);

                case (w_ir)
                LDA_a, LDA_zp_ind_y, LDA_zp, LDA_zp_x, LDA_zp_ind_x: o_sb_ac = 1;
                LDX_a, LDX_zp, LDX_zp_y: o_sb_x = 1;
                LDY_a, LDY_zp, LDY_zp_x: o_sb_y = 1;
                default: begin
                end
                endcase
                o_sb_db = 1;
                load_z_n_from_db(1);
            end
        end
        CMP_a, CPX_a, CPY_a,
        ADC_a, ADC_zp_x,
        SBC_a, SBC_ax, SBC_ay, SBC_zp_x, SBC_zp, SBC_zp_ind_y,
        AND_a, EOR_a, ORA_a,
        CMP_ax, CMP_ay, CMP_zp_x,
        AND_ax, AND_ay, AND_zp, AND_zp_x,
        ORA_ax, ORA_ay, ORA_zp_x,
        EOR_ax, EOR_ay, EOR_zp, EOR_zp_x:
        begin
            if (w_phi1)
            begin
                // load operand into ALU via DB from DL
                o_dl_db = 1;

                // load register into ALU via SB
                case (w_ir)
                CPX_a: o_x_sb = 1;
                CPY_a: o_y_sb = 1;
                default: o_ac_sb = 1;
                endcase
                    
                o_sb_add = 1;

                case (w_ir)
                AND_a, AND_ax, AND_ay, AND_zp, AND_zp_x: begin
                    o_db_add = 1;
                    o_ands = 1;
                end
                EOR_a, EOR_ax, EOR_ay, EOR_zp, EOR_zp_x: begin
                    o_db_add = 1;
                    o_eors = 1;
                end
                ORA_a, ORA_ax, ORA_ay, ORA_zp_x: begin
                    o_db_add = 1;
                    o_ors = 1;
                end
                ADC_a, ADC_zp_x: begin
                    o_db_add = 1;
                    o_sums = 1;

                    // carry in
                    o_1_addc = i_p[C];
                end
                SBC_a, SBC_ax, SBC_ay, SBC_zp_x, SBC_zp, SBC_zp_ind_y: begin
                    o_db_n_add = 1;
                    o_sums = 1;
                    
                    // carry in
                    o_1_addc = i_p[C];
                end
                CMP_a, CPX_a, CPY_a, CMP_ax, CMP_ay, CMP_zp_x: begin
                    // subtraction as 2's complement addition
                    o_db_n_add = 1;
                    o_1_addc = 1;

                    o_sums = 1;
                end
                default: begin
                end
                endcase
            end
            else
            begin
                // PHI2
                output_add_on_sb(1);

                // load Accumulator from SB
                case (w_ir)
                AND_a, AND_ax, AND_ay, AND_zp, AND_zp_x,
                EOR_a, EOR_ax, EOR_ay, EOR_zp, EOR_zp_x,
                ORA_a, ORA_ax, ORA_ay, ORA_zp_x,
                ADC_a, ADC_zp_x,
                SBC_a, SBC_ax, SBC_ay, SBC_zp_x, SBC_zp,
                SBC_zp_ind_y: begin
                    o_sb_ac = 1;
                end
                default: begin
                end
                endcase

                // load carry/overflow flags
                case (w_ir)
                CMP_a, CPX_a, CPY_a, CMP_ax, CMP_ay, CMP_zp_x:
                begin
                    // C flag
                    o_acr_c = 1;
                end
                ADC_a, ADC_zp_x,
                SBC_a, SBC_ax, SBC_ay, SBC_zp_x, SBC_zp, SBC_zp_ind_y: 
                begin
                    // C + V flags
                    o_acr_c = 1;
                    o_avr_v = 1;
                end
                default: begin
                end
                endcase 
                
                o_sb_db = 1;
                load_z_n_from_db(1);
            end
        end
        INX, INY, DEX, DEY,
        LSR_A, ASL_A, ROL_A, ROR_A:
        begin
            if (w_phi1)
            begin
                // output register to input register A via SB
                case (w_ir)
                INX, DEX: o_x_sb = 1;
                INY, DEY: o_y_sb = 1;
                LSR_A, ASL_A, ROL_A, ROR_A: o_ac_sb = 1;
                default: begin
                end
                endcase

                o_sb_add = 1;

                case (w_ir)
                INX, INY: begin
                    // load 0 as inverted 0xFF (from precharged mosfets)        
                    o_db_n_add = 1;
            
                    // use carry-in to +1
                    o_1_addc = 1;
                    o_sums = 1;
                end
                DEX, DEY: begin
                    // load -1 as 0xFF (from precharged mosfets)
                    o_db_add = 1;
                    o_sums = 1;
                end
                LSR_A, ROR_A: begin
                    o_srs = 1;
                end
                ASL_A, ROL_A: begin
                    // load accumulator into both A and B registers
                    o_sb_db = 1;
                    o_db_add = 1;

                    // sum
                    o_sums = 1;
                end
                default: begin
                end
                endcase

                case (w_ir)
                ROR_A, ROL_A: begin
                    // use CARRY flag to drive CARRY_IN on accumulator
                    o_1_addc = i_p[C];
                end
                default: begin
                end
                endcase
            end
            else
            begin
                // PHI2

                output_add_on_sb(1);

                // load from SB into register
                case (w_ir)
                INX, DEX: begin
                    o_sb_x = 1;
                end
                INY, DEY: begin
                    o_sb_y = 1;
                end
                LSR_A, ASL_A,
                ROL_A, ROR_A: begin

                    // load carry flag from result
                    o_acr_c = 1;

                    o_sb_ac = 1;
                end
                default: begin
                end
                endcase
                
                o_sb_db = 1;
                load_z_n_from_db(1);
            end
        end
        LDA_ax, LDA_ay, LDX_ay, LDY_ax:
        begin
            if (w_phi1)
            begin
                load_add_from_dl(1);
            end
            else
            begin
                output_add_on_sb(1);
                o_sb_db = 1;

                case (w_ir)
                LDA_ax, LDA_ay: o_sb_ac = 1;
                LDX_ay: o_sb_x = 1;
                LDY_ax: o_sb_y = 1;
                default: begin
                end
                endcase

                load_z_n_from_db(1);

            end

        end
        BIT_a, BIT_zp: begin
            if (w_phi1)
            begin
                // load value from DL into ALU via DB
                o_dl_db = 1;
                o_db_add = 1;

                // load value from AC into ALU
                o_ac_sb = 1;
                o_sb_add = 1;

                // use ALU for AND
                o_ands = 1;
            end
            else
            begin
                // phi 2
                
                // output AND result from ALU to DB via SB
                output_add_on_sb(1);
                o_sb_db = 1;

                // set Z if ALU result on DB is zero
                o_dbz_z = 1;
            end
        end
        default: begin
        end
        endcase
    end
    1: // T1
    begin
        case (w_ir)
        BRK, PHA, PHP, PLA, PLP, RTI: 
        begin
            // PC + 1
            retain_pc(1);
            output_pch_on_abh(1);
            output_pcl_on_abl(1);

            case (w_ir)
            PLA, PLP: begin
                // use ALU to increment the SP
                o_s_sb = 1;
                load_add_from_sb(1);
                o_1_addc = 1;
            end
            default: begin
                
            end
            endcase
        end
        SEC, CLC, SEI, CLI, CLV, SED, CLD:
        begin
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
            retain_pc(1);

            case (w_ir)
            SEC, CLC: o_ir5_c = 1; // read C from IR5
            SEI, CLI: o_ir5_i = 1; // read I from IR5
            CLV: o_avr_v = 1; // read V from (0) avr
            SED, CLD: o_ir5_d = 1; // read D from IR5
            default: begin
            end
            endcase

            next_opcode();
        end
        INX, INY, DEX, DEY,
        LSR_A, ASL_A, ROL_A, ROR_A:
        begin
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
            retain_pc(1);

            next_opcode();
        end
        LDA_i, LDX_i, LDY_i:
        begin
            output_pcl_on_abl(1);
            output_pch_on_abh(1);
            retain_pc(1);
            increment_pc(1);

            // input DL into SB via DB
            o_dl_db = 1;
            o_sb_db = 1;

            case (w_ir)
            LDA_i: o_sb_ac = 1;
            LDX_i: o_sb_x = 1;
            LDY_i: o_sb_y = 1;
            default: begin
            end
            endcase

            load_z_n_from_db(1);

            next_opcode();
        end
        AND_i, EOR_i, ORA_i, ADC_i, SBC_i,
        CMP_i, CPX_i, CPY_i:
        begin
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
            retain_pc(1);
            increment_pc(1);

            next_opcode();
        end
        LDA_a, LDX_a, LDY_a,
        STA_a, STX_a, STY_a,
        BIT_a, JMP_a, JSR_a,
        INC_a, DEC_a, JMP_indirect,
        CMP_a, CPX_a, CPY_a,
        ADC_a, SBC_a,
        AND_a, EOR_a, ORA_a,
        ASL_a, LSR_a, ROL_a, ROR_a,
        LDA_ax, LDA_ay, LDX_ay, LDY_ax,
        CMP_ax, CMP_ay,
        AND_ax, AND_ay,
        ORA_ax, ORA_ay,
        EOR_ax, EOR_ay,
        STA_ax, STA_ay,
        STA_zp, STX_zp, STY_zp,
        STA_zp_ind_y,
        ORA_zp, SBC_ax, SBC_ay,
        INC_zp, DEC_zp,
        EOR_zp, AND_zp,
        ROR_ax, ROL_ax,
        LDA_zp, LDX_zp, LDY_zp,
        LDY_zp_x, LDA_zp_x, LDX_zp_y,
        STY_zp_x, STA_zp_x, STX_zp_y,
        ORA_zp_x, AND_zp_x, EOR_zp_x,
        ADC_zp_x, CMP_zp_x, SBC_zp_x,
        ADC_zp, ADC_ax, ADC_ay,
        BIT_zp, ROL_zp, ROR_zp,
        LSR_zp, ASL_zp,
        ASL_ax, LSR_ax,
        DEC_ax, INC_ax,
        CMP_zp, CPX_zp, CPY_zp,
        DEC_zp_x, INC_zp_x,
        SBC_zp,
        ASL_zp_x, ROL_zp_x, LSR_zp_x, ROR_zp_x:
        begin
            // Read PC+1
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
            retain_pc(1);
            increment_pc(1);

            if (w_ir == JSR_a)
            begin
                // move stack pointer to alu
                o_s_sb = w_phi1;
                load_add_from_sb(w_phi1);

                // stash target address lo in S
                load_s_from_dl(w_phi2);
            end
        end
        RTS: begin
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
        end
        TAX, TAY, TXA, TYA, TXS, TSX: begin
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
            retain_pc(1);

            case (w_ir)
            TAX: begin
                o_ac_sb = 1;
                o_sb_x = 1;
            end
            TAY: begin
                o_ac_sb = 1;
                o_sb_y = 1;
            end
            TXA: begin
                o_x_sb = 1;
                o_sb_ac = 1;
            end
            TYA: begin
                o_y_sb = 1;
                o_sb_ac = 1;
            end
            TXS: begin
                o_x_sb = 1;
                o_sb_s = 1;
            end
            TSX: begin
                o_s_sb = 1;
                o_sb_x = 1;
            end
            default: begin
            end
            endcase

            // status flags
            if (w_ir != TXS)
            begin
                o_sb_db = 1;
                load_z_n_from_db(1);
            end
            
            next_opcode();
        end
        NOP:
        begin
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
            retain_pc(1);
            next_opcode();
        end
        BCC, BCS, BEQ, BNE, BMI, BPL, BVC, BVS: 
        begin
            // PC + 1 - fetch branch offset
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
            retain_pc(1);
            increment_pc(1);      

            if ( ((i_p[C] == 0) && (w_ir == BCC)) ||
                 ((i_p[C] == 1) && (w_ir == BCS)) ||
                 ((i_p[Z] == 1) && (w_ir == BEQ)) ||
                 ((i_p[Z] == 0) && (w_ir == BNE)) ||
                 ((i_p[N] == 1) && (w_ir == BMI)) ||
                 ((i_p[N] == 0) && (w_ir == BPL)) ||
                 ((i_p[V] == 0) && (w_ir == BVC)) ||
                 ((i_p[V] == 1) && (w_ir == BVS)) )
            begin
                // follow branch in T2

                // check if offset is negative with r_bus_db_n
                o_dl_db = 1;
            end
            else
            begin
                // branch not taken
                next_opcode();
            end 
        end
        LDA_zp_ind_y, ADC_zp_ind_y, CMP_zp_ind_y, SBC_zp_ind_y,
        ORA_zp_ind_y, AND_zp_ind_y, EOR_zp_ind_y,
        ORA_zp_ind_x, AND_zp_ind_x, EOR_zp_ind_x,
        ADC_zp_ind_x, SBC_zp_ind_x, CMP_zp_ind_x,
        LDA_zp_ind_x, STA_zp_ind_x:
        begin
            // load page zero indirect address
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
            retain_pc(1);
            increment_pc(1);
        end
        default:
        begin
            // unsupported opcode

            // prevent tcu from increasing
            o_tcu = i_tcu;

            // report error
            o_error = 1;

            // retain the PC
            retain_pc(1);
        end
        endcase
    end
    2: // T2
    begin
        case (w_ir)
        BCC, BCS, BEQ, BNE, BMI, BPL, BVC, BVS:
        begin
            // PC + 2
            output_pch_on_abh(1);
            retain_pch(1);
            
            // DL contains branch offset

            if (w_phi1)
            begin
                // phi1
                o_dl_db = 1;

                output_pcl_on_abl(1);

                // use ALU to add relative offset to PC+2
                o_sums = 1;

                // PC+2 (lo)
                o_adl_add = 1;

                // + relative
                o_sb_db = 1;
                o_sb_add = 1;
            end 
            else
            begin
                // phi2

                // write result of ALU to PCL
                o_add_adl = 1;
                o_adl_pcl = 1;
                
                if (i_acr == r_bus_db_n)
                begin
                    // we don't need to carry, so transition to next
                    // T0 where ADD should be output to PCL / ABL

                    next_opcode();
                end
            end
        end
        JSR_a:
        begin
            retain_pc(1);           // PC + 2

            output_add_on_abl(1);   // output SP on address
            output_1_on_abh(1);
            load_add_from_adl(1);   // retain SP in ADD
        end
        RTS, RTI:
        begin
            output_s_on_abl(1);
            output_1_on_abh(1);
            load_add_from_adl_plus_1(1);        // SP + 1
        end
        BRK, PHA, PHP:
        begin
            retain_pc(1);

            output_s_on_abl(w_phi1);
            output_1_on_abh(w_phi1);

            load_add_from_adl_minus_1(w_phi1);       // SP - 1

            if (w_ir == PHA)
            begin
                o_rw = RW_WRITE;
                o_ac_db = 1;
                next_opcode();
                load_s_from_add(w_phi2);
            end
            else if (w_ir == PHP)
            begin
                o_rw = RW_WRITE;
                o_p_db = 1;
                o_1_db4 = 1;                // force BREAK bit in P written to stack
                next_opcode();
                load_s_from_add(w_phi2);
            end
            else if (w_ir == BRK)
            begin
                if (r_hw_interrupt != HW_INTERRUPT_RESET)
                begin
                    o_rw = RW_WRITE;
                    o_pch_db = 1;
                end
            end
        end
        PLA, PLP: begin
            retain_pc(1);

            output_s_on_abl(1);
            output_1_on_abh(1);

            // write SP+1 to S
            load_s_from_add(1);

            // retain the value in ADD
            load_add_from_sb(1);
        end
        ORA_zp, LDA_zp, LDX_zp, LDY_zp, BIT_zp,
        CMP_zp, CPX_zp, CPY_zp, SBC_zp:
        begin
            retain_pc(1);
            output_0_on_abh(w_phi1);
            output_dl_on_abl(w_phi1);

            case (w_ir)
            BIT_zp:
            begin
                o_dl_db = w_phi2;
                o_db6_v = w_phi2;
                o_db7_n = w_phi2;
            end
            default: begin
            end
            endcase

            next_opcode();
        end
        STA_zp, STX_zp, STY_zp:
        begin
            retain_pc(1);
            output_0_on_abh(1);
            output_dl_on_abl(1);

            o_rw = RW_WRITE;

            case (i_ir)
            STX_zp: o_x_sb = 1;
            STY_zp: o_y_sb = 1;
            default: o_ac_sb = 1;
            endcase

            o_sb_db = 1;

            next_opcode();
        end
        STA_zp_ind_y:
        begin
            retain_pc(1);

            output_0_on_abh(1);
            output_dl_on_abl(1);

            load_add_from_dl(1);
            o_1_addc = 1;
        end
        LDY_zp_x, LDA_zp_x, LDX_zp_y,
        STY_zp_x, STA_zp_x, STX_zp_y,
        ORA_zp_x, AND_zp_x, EOR_zp_x,
        ADC_zp_x, CMP_zp_x, SBC_zp_x,
        DEC_zp_x, INC_zp_x,
        ASL_zp_x, ROL_zp_x, LSR_zp_x, ROR_zp_x:
        begin
            retain_pc(1);

            output_0_on_abh(1);
            output_dl_on_abl(1);

            load_add_from_dl(1);
            o_0_add = 0;
            case (i_ir)
            LDX_zp_y, STX_zp_y: begin
                o_y_sb = 1;
            end
            default: begin
                o_x_sb = 1;
            end
            endcase
            o_sb_add = 1;
        end
        LDA_a, LDX_a, LDY_a,
        STA_a, STX_a, STY_a,
        BIT_a, INC_a, DEC_a,
        CMP_a, CPX_a, CPY_a,
        ADC_a, SBC_a,
        AND_a, EOR_a, ORA_a,
        ASL_a, LSR_a, ROL_a, ROR_a,
        LDA_ax, LDA_ay, LDX_ay, LDY_ax,
        CMP_ax, CMP_ay, AND_ax, AND_ay,
        ORA_ax, ORA_ay,
        EOR_ax, EOR_ay,
        STA_ax, STA_ay,
        SBC_ax, SBC_ay,
        ADC_ax, ADC_ay:
        begin
            // PC + 2 = Fetch high order effective address byte            
            retain_pc(1);
            output_pcl_on_abl(1);
            output_pch_on_abh(1);

            // load BASE ADDRESS LO from DL into ALU
            load_add_from_dl(1);

            case (w_ir)
            LDA_ax, CMP_ax, LDY_ax, AND_ax, ORA_ax, EOR_ax, STA_ax, SBC_ax, ADC_ax:
            begin
                // add index register
                o_0_add = 0;                // cancel this signal set in load_add_from_dl
                o_sb_add = 1;
                o_x_sb = 1;
            end
            LDA_ay, CMP_ay, LDX_ay, AND_ay, ORA_ay, EOR_ay, STA_ay, SBC_ay, ADC_ay:
            begin
                // add index register
                o_0_add = 0;                // cancel this signal set in load_add_from_dl
                o_sb_add = 1;
                o_y_sb = 1;
            end
            default:
            begin
            end
            endcase
        end
        ROR_ax, ROL_ax, ASL_ax, LSR_ax, DEC_ax, INC_ax:
        begin
            // PC + 2 = Fetch high order effective address byte            
            retain_pc(1);
            output_pcl_on_abl(1);
            output_pch_on_abh(1);
            increment_pc(1);

            // load BASE ADDRESS LO from DL into ALU
            load_add_from_dl(1);

            // add X to alu
            o_0_add = 0;
            o_sb_add = 1;
            o_x_sb= 1;
        end
        JMP_a, JMP_indirect:
        begin
            // PC + 2 = Fetch high order effective address byte

            if (w_phi1)
            begin
                output_pch_on_abh(1);
                output_pcl_on_abl(1);

                // load address lo from DL
                load_add_from_dl(1);
            end
            else
            begin
                load_pcl_from_add(1);       
                load_pch_from_dl(1);
            end

            if (w_ir == JMP_a)
            begin
                next_opcode();
            end
        end
        LDA_zp_ind_y, ADC_zp_ind_y, CMP_zp_ind_y, SBC_zp_ind_y,
        ORA_zp_ind_y, AND_zp_ind_y, EOR_zp_ind_y:
        begin
            retain_pc(1);

            // output 0, IAL - low byte of base address
            output_0_on_abh(1);
            output_dl_on_abl(1);

            // start increment of 
            load_add_from_dl(1);

            // increment add
            o_1_addc = 1;
            o_sums = 1;
            o_0_add = 1;
        end
        ORA_zp_ind_x, AND_zp_ind_x, EOR_zp_ind_x,
        ADC_zp_ind_x, SBC_zp_ind_x, CMP_zp_ind_x,
        LDA_zp_ind_x, STA_zp_ind_x:
        begin
            retain_pc(1);

            // output 0, BAL
            output_0_on_abh(1);
            output_dl_on_abl(1);

            // start adding x to BAL
            o_dl_db = 1;
            o_db_add = 1;
            o_x_sb = 1;
            o_sb_add = 1;
            o_sums = 1;
        end
        INC_zp, DEC_zp, EOR_zp, AND_zp, ADC_zp, ROL_zp, ROR_zp, LSR_zp, ASL_zp:
        begin
            retain_pc(1);

            // output 0, ADL - zero page effective address
            output_0_on_abh(1);
            output_dl_on_abl(1);

            case (w_ir)
            EOR_zp, AND_zp, ADC_zp:
            begin
                next_opcode();
            end
            default: begin
            end
            endcase
        end
        default:
        begin
        end
        endcase
    end
    3: // T3
    begin
        case (w_ir)
        BCC, BCS, BEQ, BNE, BMI, BPL, BVC, BVS:
        begin
            retain_pcl(1);

            if (w_phi1)
            begin
                output_pch_on_abh(1);
                output_pcl_on_abl(1);

                // use ALU to add +/-1 to ADH
                o_sums = 1;

                // B INPUT = pch
                o_pch_db = 1;
                o_db_add = 1;

                if (r_last_acr)
                begin
                    // + 1
                    o_1_addc = 1;
                    o_0_add = 1;
                end
                else
                begin
                    // - 1
                    o_sb_add = 1;
                end
            end
            else
            begin
                // write add to PCH
                output_add_on_sb(1);
                o_sb_adh = 1;
                o_adh_pch = 1;
            end

            next_opcode();
        end
        BRK:
        begin
            // output S-1
            output_add_on_abl(1);
            output_1_on_abh(1);

            load_add_from_adl_minus_1(1);       // SP -1

            if (r_hw_interrupt != HW_INTERRUPT_RESET)
            begin
                o_rw = RW_WRITE;
                o_pcl_db = 1;
            end
        end
        CMP_a, CPX_a, CPY_a,
        ADC_a, SBC_a,
        AND_a, EOR_a, ORA_a:
        begin
            // output absolute address ADH, ADL, fetch data
            retain_pc(1);
            increment_pc(1);
            
            output_add_on_abl(1);
            output_dl_on_abh(1);

            next_opcode();
        end
        LDA_a, LDX_a, LDY_a,
        STA_a, STX_a, STY_a,
        BIT_a, INC_a, DEC_a,
        ASL_a, LSR_a, ROL_a, ROR_a:
        begin
            // output absolute address ADH, ADL
            retain_pc(1);
            increment_pc(1);

            output_add_on_abl(w_phi1);
            output_dl_on_abh(w_phi1);
            
            case (w_ir)
            STA_a: begin
                // write value from AC
                o_ac_db = 1;
                o_rw = RW_WRITE;
            end
            STX_a: begin
                // write value from X
                o_x_sb = 1;
                o_sb_db = 1;
                o_rw = RW_WRITE;
            end
            STY_a: begin
                // write value from Y
                o_y_sb = 1;
                o_sb_db = 1;
                o_rw = RW_WRITE;
            end
            BIT_a: begin
                o_dl_db = 1;

                // load V and N from DB
                o_db6_v = 1;
                o_db7_n = 1;
            end
            default: begin
            end
            endcase

            case (w_ir)
            INC_a, DEC_a, ASL_a, LSR_a, ROL_a, ROR_a: begin
            end
            default: begin
                next_opcode();
            end
            endcase
        end
        LDA_ax, LDA_ay, CMP_ax, CMP_ay, LDX_ay, LDY_ax, AND_ax, AND_ay,
        ORA_ax, ORA_ay,
        EOR_ax, EOR_ay,
        STA_ax, STA_ay,
        SBC_ax, SBC_ay,
        ADC_ax, ADC_ay: begin
            output_dl_on_abh(1);        // BAH
            output_add_on_abl(1);
                        
            retain_pc(1);
            increment_pc(1);

            if (r_last_acr == 1)
            begin
                // use ALU to add +1 to Base address high (BAH)

                // B INPUT = (BAH)
                load_add_from_dl(1);

                // + 1
                o_1_addc = 1;
            end
            else
            begin
                case (w_ir)
                STA_ax, STA_ay:
                begin
                    // continue to T4
                    load_add_from_dl(1);
                end
                default:
                begin
                    next_opcode();
                end
                endcase
            end
        end
        PLA, PLP: begin
            retain_pc(1);

            // output SP+1
            output_s_on_abl(1);
            output_1_on_abh(1);

            if (w_ir == PLA) begin
                load_ac_from_dl(1);
                o_dbz_z = 1;
                o_db7_n = 1;
            end
            else if (w_ir == PLP) begin
                o_dl_db = 1;
                load_p_from_db(1);
            end
        
            next_opcode();
        end
        JSR_a:
        begin
            retain_pc(1);       // PC + 2

            // output S on address bus
            output_add_on_abl(1);
            output_1_on_abh(1);
            
            // write PCH to data bus
            o_rw = RW_WRITE;
            o_pch_db = 1;

            load_add_from_adl_minus_1(1);       // SP - 1
        end
        RTS, RTI:
        begin
            // output S+1
            output_add_on_abl(1);
            output_1_on_abh(1);

            load_add_from_adl_plus_1(1);       // SP + 1

            // load from SP+1
            o_dl_db = 1;

            case (w_ir)
            RTS:
            begin
                // load return address (lo) into SP temporarily    
                o_sb_db = 1;
                o_sb_s = 1;
            end
            RTI:
            begin
                load_p_from_db(1);
            end
            default: begin
            end
            endcase
        end
        JMP_indirect:
        begin
            // Indirect address = Fetch low order effective address byte
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
            retain_pc(1);
            
            // increment LSB
            output_add_on_sb(1);
            o_sb_db = 1;
            o_db_add = 1;
            o_sums = 1;
            o_0_add = 1;
            o_1_addc = 1;
        end
        STA_zp_ind_y:
        begin
            retain_pc(1);

            output_0_on_abh(1);
            output_add_on_abl(1);

            // add address.lo to Y index register
            o_dl_db = 1;
            o_db_add = 1;
            o_y_sb = 1;
            o_sb_add = 1;
            o_sums = 1;
        end
        LDA_zp_ind_y, ADC_zp_ind_y, CMP_zp_ind_y, SBC_zp_ind_y,
        ORA_zp_ind_y, AND_zp_ind_y, EOR_zp_ind_y:
        begin
            retain_pc(1);

            // output 0, IAL + 1 on address bus = fetch high byte of base address
            output_0_on_abh(1);
            output_add_on_abl(1);

            // load low byte of base address into ADD
            load_add_from_dl(1);
            
            // increment add by Y
            o_sums = 1;
            o_0_add = 0;                // cancel this signal set in load_add_from_dl
            o_y_sb = 1;
            o_sb_add = 1;
        end
        ORA_zp_ind_x, AND_zp_ind_x, EOR_zp_ind_x,
        ADC_zp_ind_x, SBC_zp_ind_x, CMP_zp_ind_x,
        LDA_zp_ind_x, STA_zp_ind_x:
        begin
            retain_pc(1);

            // output 0, BAL+x on address bus = fetch low order byte of effective address
            output_0_on_abh(1);
            output_add_on_abl(1);

            // increment ADD by 1 to get BAL+x+1
            o_sums = 1;
            o_0_add = 1;
            o_1_addc = 1;
            output_add_on_sb(1);
            o_sb_db = 1;
            o_db_add = 1;
        end
        INC_zp, DEC_zp, ROL_zp, ROR_zp, LSR_zp, ASL_zp:
        begin
            retain_pc(1);

            if (w_phi1)
            begin
                load_add_from_dl(1);
                
                case (w_ir)
                INC_zp:
                begin
                    // increment value read from ZP
                    o_1_addc = 1;
                    o_sums = 1;
                    o_0_add = 1; 
                end
                DEC_zp:
                begin
                    // decrement value read from zp
                    o_sums = 1;
                    o_sb_add = 1; // precharge mosfets
                    o_0_add = 0;
                end
                ROL_zp, ASL_zp:
                begin
                    // rotate value to the left
                    o_sb_db = 1;
                    o_sb_add = 1;
                    o_0_add = 0;

                    // load accumulator into both A and B registers
                    o_db_add = 1;

                    // sum
                    o_sums = 1;

                    if (w_ir == ROL_zp)
                    begin
                        // use CARRY flag to drive CARRY_IN on ALU
                        o_1_addc = i_p[C];
                    end
                end
                ROR_zp, LSR_zp:
                begin
                    // rotate value to the right
                    o_sb_db = 1;
                    o_sb_add = 1;
                    o_0_add = 0;
                    o_sums = 0;
                    o_srs = 1;

                    if (w_ir == ROR_zp)
                    begin
                        // use CARRY flag to drive CARRY_IN on ALU
                        o_1_addc = i_p[C];
                    end
                end
                default: begin
                end
                endcase
            end
            else 
            begin
                output_add_on_sb(1);
                o_sb_db = 1;
                case (w_ir)
                ROL_zp, ROR_zp, LSR_zp, ASL_zp:
                begin
                    o_acr_c = 1;
                end
                default: begin
                end
                endcase
                load_z_n_from_db(1);
            end

            o_rw = RW_WRITE;
        end
        ROR_ax, ROL_ax, ASL_ax, LSR_ax, DEC_ax, INC_ax:
        begin
            retain_pc(1);

            output_add_on_abl(1);
            output_dl_on_abh(1);

            if (r_last_acr)
            begin
                o_1_addc = 1;
            end
            
            // load BASE ADDRESS HI from DL into ALU
            load_add_from_dl(1);
        end
        LDY_zp_x, LDA_zp_x, LDX_zp_y,
        ORA_zp_x, AND_zp_x, EOR_zp_x,
        ADC_zp_x, CMP_zp_x, SBC_zp_x:
        begin
            retain_pc(1);

            output_0_on_abh(1);
            output_add_on_abl(1);

            next_opcode();
        end
        DEC_zp_x, INC_zp_x,
        ASL_zp_x, ROL_zp_x, LSR_zp_x, ROR_zp_x:
        begin
            retain_pc(1);

            output_0_on_abh(1);
            output_add_on_abl(1);

            load_add_from_dl(1);
        end
        STY_zp_x, STA_zp_x, STX_zp_y:
        begin
            retain_pc(1);

            output_0_on_abh(1);
            output_add_on_abl(1);

            o_rw = RW_WRITE;

            case (i_ir)
            STY_zp_x:   o_y_sb = 1;
            STX_zp_y:   o_x_sb = 1;
            default:    o_ac_sb = 1;
            endcase

            o_sb_db = 1;

            next_opcode();
        end
        default:
        begin
        end
        endcase
    end
    4: // T4
    begin
        case (w_ir)
        BRK:
        begin
            // output S-2
            output_add_on_abl(w_phi1);
            output_1_on_abh(1);

            load_add_from_adl_minus_1(w_phi1);      // S-3

            load_s_from_add(w_phi2);     // SP - 3
        
            if (r_hw_interrupt != HW_INTERRUPT_RESET)
            begin
                o_rw = RW_WRITE;
                o_p_db = 1;

                if (r_hw_interrupt == HW_INTERRUPT_NONE)
                begin
                    o_1_db4 = 1;        // force B (BREAK) bit in P written to stack
                end
            end
        end
        JSR_a:
        begin
            // T4
            retain_pc(1);

            // output S-1
            output_add_on_abl(1);
            output_1_on_abh(1);

            // write PCL to data bus
            o_rw = RW_WRITE;
            o_pcl_db = 1;

            load_add_from_adl_minus_1(1);       // S-2
        end
        RTS:
        begin
            if (w_phi1)
            begin
                // output S+2
                output_add_on_abl(1);
                output_1_on_abh(1);

                // retain s+2
                load_add_from_adl(1);
            end
            else
            begin
                load_pcl_from_s(1);        // return address lo
                load_pch_from_dl(1);       // return address hi
            
                // read S+2 into SP from ALU
                load_s_from_add(1);
            end
        end
        RTI:
        begin
            // output S+2
            output_add_on_abl(1);
            output_1_on_abh(1);

            load_add_from_adl_plus_1(1);        // SP + 3

            load_s_from_dl(1);                  // stash PCL temporarily
        end
        INC_a, DEC_a, ASL_a, LSR_a, ROL_a, ROR_a: 
        begin
            retain_pc(1);

            // note: the correct address is already buffered in ABL/ABH

            o_rw = RW_WRITE;

            if (w_phi1)
            begin
                o_dl_db = 1;

                case (w_ir)
                INC_a: 
                begin
                    // increment ALU
                    o_db_add = 1;
                    o_1_addc = 1;
                    o_sums = 1;
                    o_0_add = 1; 
                end
                DEC_a:
                begin
                    // decrement ALU 
                    o_sb_db = 1;
                    o_sb_add = 1;
                    o_sums = 1;
                    o_adl_add = 1;  // 0xFF from precharge mosfets
                end
                LSR_a, ROR_a: begin
                    o_sb_db = 1;
                    o_sb_add = 1;

                    o_srs = 1;
                end
                ASL_a, ROL_a: begin
                    o_sb_db = 1;
                    o_sb_add = 1;

                    // load accumulator into both A and B registers
                    o_db_add = 1;

                    // sum
                    o_sums = 1;
                end
                default:
                begin
                end
                endcase

                case (w_ir)
                ROR_a, ROL_a: begin
                    // use CARRY flag to drive CARRY_IN on ALU
                    o_1_addc = i_p[C];
                end
                default: begin
                end
                endcase
            end
            else
            begin
                output_add_on_sb(1);
                o_sb_db = 1;
                load_z_n_from_db(1);

                case (w_ir)
                ASL_a, ROL_a, LSR_a, ROR_a:
                begin
                    // load carry into status register
                    o_acr_c = 1;
                end
                default:
                begin
                end
                endcase
            end
        end
        JMP_indirect:
        begin
            // indirect address high - load high order byte of jump address

            if (w_phi1)
            begin
                output_pch_on_abh(1);
                output_add_on_abl(1);
                load_add_from_dl(1);        // address lo
            end
            else
            begin
                load_pcl_from_add(1);
                load_pch_from_dl(1);
            end

            next_opcode();
        end
        LDA_ax, LDA_ay, CMP_ax, CMP_ay, LDX_ay, LDY_ax, AND_ax, AND_ay,
        ORA_ax, ORA_ay,
        EOR_ax, EOR_ay,
        STA_ax, STA_ay,
        SBC_ax, SBC_ay,
        ADC_ax, ADC_ay:
        begin
            retain_pc(1);
            output_add_on_abh(1);

            case (w_ir)
            STA_ax, STA_ay:
            begin
                o_rw = RW_WRITE;
                o_ac_db = 1;
            end
            default:
            begin
            end
            endcase

            next_opcode();
        end
        STA_zp_ind_y:
        begin
            retain_pc(1);

            output_dl_on_abh(1);
            output_add_on_abl(1);

            load_add_from_dl(1);

            o_1_addc = r_last_acr;
        end
        LDA_zp_ind_y, ADC_zp_ind_y, CMP_zp_ind_y, SBC_zp_ind_y,
        ORA_zp_ind_y, AND_zp_ind_y, EOR_zp_ind_y:
        begin
            retain_pc(1);

            output_dl_on_abh(1);
            output_add_on_abl(1);

            if (r_last_acr == 1)
            begin
                // use ALU to add +1 to Base address high (BAH)

                // B INPUT = (BAH)
                load_add_from_dl(1);

                // + 1
                o_1_addc = 1;
            end
            else
            begin
                next_opcode();
            end
        end
        ORA_zp_ind_x, AND_zp_ind_x, EOR_zp_ind_x,
        ADC_zp_ind_x, SBC_zp_ind_x, CMP_zp_ind_x,
        LDA_zp_ind_x, STA_zp_ind_x:
        begin
            retain_pc(1);

            output_0_on_abh(1);
            output_add_on_abl(1);           // BAL + x + 1

            load_add_from_dl(1);            // cache low order byte of effective address
        end
        INC_zp, DEC_zp, ROL_zp, ROR_zp, LSR_zp, ASL_zp:
        begin
            retain_pc(1);

            output_add_on_sb(w_phi1);
            o_sb_db = w_phi1;

            o_rw = RW_WRITE;

            next_opcode();
        end
        ROR_ax, ROL_ax, ASL_ax, LSR_ax, DEC_ax, INC_ax:
        begin
            retain_pc(1);

            output_add_on_abh(1);
        end
        DEC_zp_x, INC_zp_x,
        ASL_zp_x, ROL_zp_x, LSR_zp_x, ROR_zp_x:
        begin
            retain_pc(1);

            o_rw = RW_WRITE;
            
            if (w_phi1)
            begin
                o_dl_db = 1;
                o_sb_db = 1;
                
                case (w_ir)
                ROR_zp_x, LSR_zp_x:
                begin
                    // shift right
                    o_sb_add = 1;

                    o_srs = 1;    
                end
                ROL_zp_x, ASL_zp_x:
                begin
                    // shift left
                
                    // load accumulator into both A and B registers
                    o_db_add = 1;
                    o_sb_add = 1;

                    // sum
                    o_sums = 1;
                end
                DEC_zp_x:
                begin
                    // decrement
                    o_sums = 1;
                    o_db_add = 1;
                    
                    o_adl_add = 1;  // 0xFF from precharge mosfets
                end
                INC_zp_x:
                begin
                    // increment
                    o_sums = 1;
                    o_db_add = 1;
                    o_1_addc = 1;
                    o_0_add = 1;
                end
                default: begin
                end
                endcase

                case (w_ir)
                ROR_zp_x, ROL_zp_x: begin
                    // use CARRY flag to drive CARRY_IN on accumulator
                    o_1_addc = i_p[C];
                end
                default: begin
                end
                endcase
            end
            else
            begin
                output_add_on_sb(1);
                
                case (w_ir)
                ROR_zp_x, LSR_zp_x,
                ROL_zp_x, ASL_zp_x:
                begin
                    // load carry flag from result
                    o_acr_c = 1;
                end
                default: begin
                end
                endcase

                o_sb_db = 1;
                load_z_n_from_db(1);
            end
        end
        default:
        begin
        end
        endcase
    end
    5: // T5
    begin
        case (w_ir)
        BRK:
        begin
            // address of interrupt vector low byte
            load_abh_with_ff(1);
            
            case (r_hw_interrupt)
            HW_INTERRUPT_RESET:
            begin
                // ABL = 0xfc
                o_0_adl0 = 1;
                o_0_adl1 = 1;
            end
            HW_INTERRUPT_NMI:
            begin
                // ABL = 0xfa
                o_0_adl0 = 1;
                o_0_adl2 = 1;
            end
            default:
            begin
                // ABL = 0xFE
                o_0_adl0 = 1;
            end
            endcase

            o_adl_abl = 1;
        end
        RTI:
        begin
            if (w_phi1)
            begin
                // output SP + 3
                output_add_on_abl(1);
                output_1_on_abh(1);

                load_add_from_adl(1);
            end
            else
            begin
                load_pcl_from_s(1);     // return address lo
                load_pch_from_dl(1);    // return address hi

                // read S+3 into SP from ALU
                load_s_from_add(1);
            end

            next_opcode();
        end
        JSR_a:
        begin
            // output PC + 2 on address
            output_pch_on_abh(w_phi1);
            output_pcl_on_abl(w_phi1);

            if (w_phi1)
            begin
                // retain add (sp-2)
                output_add_on_sb(1);
                load_add_from_sb(1);
            end
            else
            begin
                // phi 2
                load_pcl_from_s(1);
                load_pch_from_dl(1);
                load_s_from_add(1);
            end
            next_opcode();
        end
        RTS:
        begin
            output_pch_on_abh(1);
            output_pcl_on_abl(1);
            retain_pc(1);
            increment_pc(1);

            next_opcode();
        end
        INC_a, DEC_a, ASL_a, LSR_a, ROL_a, ROR_a:
        begin
            retain_pc(1);

            // write from ALU
            o_rw = RW_WRITE;
            output_add_on_sb(w_phi1);
            o_sb_db = w_phi1;

            next_opcode();
        end
        STA_zp_ind_y:
        begin
            retain_pc(1);

            output_add_on_abh(1);

            o_rw = RW_WRITE;
            o_ac_db = 1;

            next_opcode();            
        end
        LDA_zp_ind_y, ADC_zp_ind_y, CMP_zp_ind_y, SBC_zp_ind_y,
        ORA_zp_ind_y, AND_zp_ind_y, EOR_zp_ind_y:
        begin
            retain_pc(1);

            output_add_on_abh(1);

            next_opcode();
        end
        ORA_zp_ind_x, AND_zp_ind_x, EOR_zp_ind_x,
        ADC_zp_ind_x, SBC_zp_ind_x, CMP_zp_ind_x,
        LDA_zp_ind_x, STA_zp_ind_x:
        begin
            retain_pc(1);

            output_add_on_abl(1);
            o_dl_adh = 1;
            o_adh_abh = 1;

            if (w_ir == STA_zp_ind_x)
            begin
                o_rw = RW_WRITE;
                o_ac_db = 1;
            end

            next_opcode();
        end
        ROR_ax, ROL_ax, ASL_ax, LSR_ax, DEC_ax, INC_ax:
        begin
            retain_pc(1);

            if (w_phi1)
            begin
                o_dl_db = 1;
                o_sb_db = 1;
                
                case (w_ir)
                ROR_ax, LSR_ax:
                begin
                    // shift right
                    o_sb_add = 1;

                    o_srs = 1;    
                end
                ROL_ax, ASL_ax:
                begin
                    // shift left
                
                    // load accumulator into both A and B registers
                    o_db_add = 1;
                    o_sb_add = 1;

                    // sum
                    o_sums = 1;
                end
                DEC_ax:
                begin
                    // decrement
                    o_sums = 1;
                    o_db_add = 1;
                    
                    o_adl_add = 1;  // 0xFF from precharge mosfets
                end
                INC_ax:
                begin
                    // increment
                    o_sums = 1;
                    o_db_add = 1;
                    o_1_addc = 1;
                    o_0_add = 1;
                end
                default: begin
                end
                endcase

                case (w_ir)
                ROR_ax, ROL_ax: begin
                    // use CARRY flag to drive CARRY_IN on accumulator
                    o_1_addc = i_p[C];
                end
                default: begin
                end
                endcase
            end
            else
            begin
                output_add_on_sb(1);
                
                case (w_ir)
                ROR_ax, LSR_ax,
                ROL_ax, ASL_ax:
                begin
                    // load carry flag from result
                    o_acr_c = 1;
                end
                default: begin
                end
                endcase

                o_sb_db = 1;
                load_z_n_from_db(1);
            end

            o_rw = RW_WRITE;
        end
        DEC_zp_x, INC_zp_x,
        ASL_zp_x, ROL_zp_x, LSR_zp_x, ROR_zp_x:
        begin
            retain_pc(1);

            o_rw = RW_WRITE;
            
            output_add_on_sb(1);

            o_sb_db = 1;

            next_opcode();
        end
        default:
        begin
            
        end
        endcase
    end
    6: // T6
    begin
        case (w_ir)
        BRK:
        begin
            // output address for reset vector hi

            load_abh_with_ff(1);

            case (r_hw_interrupt)
            HW_INTERRUPT_RESET:
            begin
                // ABL = 0xfd
                o_0_adl1 = w_phi1;
            end
            HW_INTERRUPT_NMI:
            begin
                // ABL = 0xfb
                o_0_adl2 = w_phi1;
            end
            default:
            begin
                // ABL = 0xFF
                // (use precharge mosfets without modification)
            end
            endcase
            
            o_adl_abl = w_phi1;
            
            // load value of reset vector lo into the ALU
            load_add_from_dl(w_phi1);

            // load reset vector lo from ADD into the PCL
            load_pcl_from_add(w_phi2);

            // load reset vector hi from DL straight into PCH
            load_pch_from_dl(w_phi2);
        
            case (r_hw_interrupt)
            HW_INTERRUPT_IRQ, HW_INTERRUPT_NMI, HW_INTERRUPT_RESET:
            begin
                // hardware interrupt
                o_db2_i = 1;
            end
            default: 
            begin
            end
            endcase

            next_opcode();
        end
        ROR_ax, ROL_ax, ASL_ax, LSR_ax, DEC_ax, INC_ax:
        begin
            retain_pc(1);

            output_add_on_sb(1);
            o_sb_db = 1;

            o_rw = RW_WRITE;

            next_opcode();
        end
        default:
        begin
            
        end
        endcase
    end
    7: // T7
    begin
        case (w_ir)
        default: begin
            
        end
        endcase
    end
    default:
    begin
        // unknown TCU state
    end
    endcase
end

endmodule
