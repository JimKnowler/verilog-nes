// Model the Decode Rom in 6502
// -> combinatorial logic mapping (IR, TCU, P) => control lines
//

// TODO: use 'functions' to enapsulate repeated combinatorial flags
//       simplify code, and self-document! 

module Decoder(
    input i_reset_n,
    input i_clk,
    
    input [7:0] i_ir,               // Instruction Register
    input [3:0] i_tcu,              // Opcode timing
    input [7:0] i_p,                // Processor status register
    input i_acr,                    // ALU - carry out

    output reg [3:0] o_tcu,         // value TCU at next phi2 clock tick

    // control signals
    output reg o_rw,
    output reg o_dl_db,
    output reg o_dl_adl,
    output reg o_dl_adh,
    output reg o_pcl_pcl,
    output reg o_adl_pcl,
    output reg o_i_pc,
    output reg o_pclc,
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
    output reg o_avr_v
);

// Processor Status Register bitfields
localparam C = 0;       // Carry
localparam Z = 1;       // Zero
localparam N = 7;       // Negative
localparam V = 6;       // Overflow

// register clock phase - so that Decoder can use clock
//    phase in combinatorial logic, without risking timing
//    issues with any of the latches (ABH/ABH/DL/etc)
reg r_clk;
always @(posedge i_clk or negedge i_clk)
begin
    r_clk <= i_clk;
end

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
                 JMP_indirect = 8'h6C;

// RW pin
localparam RW_READ = 1;
localparam RW_WRITE = 0;

// cache acr (alu carry-out) from last cycle
reg r_last_acr;
always @(negedge i_clk)
begin
    r_last_acr <= i_acr;
end

// distinguish between RESET interrupt and normal BRK 
// .. and IRQ and NMI
// -> local reg storing interrupt type
//    - set to RESET when reset 
//    - set to IRQ / NMI later when they are used
//    - reset to NONE when 


localparam INTERRUPT_NONE = 0;
localparam INTERRUPT_RESET = 1;
localparam INTERRUPT_IRQ = 2;
localparam INTERRUPT_NMI = 3;

reg [1:0] r_interrupt;

always @(negedge i_reset_n or negedge i_clk)
begin
    if (!i_reset_n)
    begin
        r_interrupt <= INTERRUPT_RESET;
    end
    else
    begin
        if (i_tcu == 6)
        begin
            // end of T6 for interrupt handling
            r_interrupt <= INTERRUPT_NONE;
        end

        // todo: at start of phi1 for T0, 
        //       check for NMI or IRQ lines
    end
end

// w_ir => will usually pass through i_ir, but will report
//         BRK while handling an interrupt
wire [7:0] w_ir;
assign w_ir = (r_interrupt == INTERRUPT_NONE) ? i_ir : BRK;

always @(*)
begin
    // default TCU to increment at next clock tick
    o_tcu = i_tcu + 1;

    // default all control signals
    o_rw = RW_READ;
    o_dl_db = 0;
    o_dl_adl = 0;
    o_dl_adh = 0;
    o_pcl_pcl = 0;
    o_adl_pcl = 0;
    o_i_pc = 0;
    o_pclc = 0;
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
    o_avr_v = 0;

    case (i_tcu)
    0:  // T0
    begin        
        // fetch

        
        // output PCL on ABL
        o_pcl_adl = 1;
        o_adl_abl = 1;

        // output PCH on ABH
        o_pch_adh = 1;
        o_adh_abh = 1;

        // retain PCL and PCH
        o_pcl_pcl = 1;
        o_pch_pch = 1;

        // increment PC for T1
        o_i_pc = 1;

        // finish previous opcode
        case (w_ir)
        INX, INY, DEX, DEY,
        LSR_A, ASL_A, ROL_A, ROR_A,
        AND_i, EOR_i, ORA_i,
        ADC_i, SBC_i,
        CMP_i, CPX_i, CPY_i: begin
            // output value from ALU to SB
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;

            // load from SB into register
            case (w_ir)
            INX, DEX: begin
                o_sb_x = 1;
            end
            INY, DEY: begin
                o_sb_y = 1;
            end
            LSR_A, ASL_A,
            ROL_A, ROR_A,
            AND_i, EOR_i, 
            ORA_i, ADC_i,
            SBC_i: begin
                o_sb_ac = 1;
            end
            default: begin
            end
            endcase
            
            // load Z & N status flags from SB via DB
            o_sb_db = 1;
            o_db7_n = 1;
            o_dbz_z = 1;
        end
        PHA, PHP: begin
            // store modified SP from ALU in S
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;

            o_sb_s = 1;
        end
        BIT_a: begin
            // output AND result from ALU to DB via SB
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_db = 1;

            // set Z if ALU result on DB is zero
            o_dbz_z = 1;
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
            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;
            
            // output PCL on ABL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // output PCH on ABH
            o_pch_adh = 1;
            o_adh_abh = 1;

            case (w_ir)
            PLA, PLP: begin
                // use ALU to increment the SP
                o_s_sb = 1;
                o_sb_add = 1;
                o_db_n_add = 1;
                o_1_addc = 1;
                o_sums = 1;
            end
            default: begin
                
            end
            endcase
        end
        SEC, CLC, SEI, CLI, CLV:
        begin
            // high byte - from PCH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // low byte - from PCL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            case (w_ir)
            SEC, CLC: o_ir5_c = 1; // read C from IR5
            SEI, CLI: o_ir5_i = 1; // read I from IR5
            CLV: o_avr_v = 1; // read V from (0) avr
            default: begin
            end
            endcase

            // start next instruction
            o_tcu = 0;
        end
        INX, INY, DEX, DEY,
        LSR_A, ASL_A, ROL_A, ROR_A:
        begin
            // high byte - from PCH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // low byte - from PCL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

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

                // load carry flag during the calculation
                o_acr_c = 1;
            end
            ASL_A, ROL_A: begin
                // load accumulator into both A and B registers
                o_sb_db = 1;
                o_db_add = 1;

                // sum
                o_sums = 1;

                // load carry into status register
                o_acr_c = 1;
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

            // end of opcode
            o_tcu = 0;
        end
        LDA_i, LDX_i, LDY_i:
        begin
            // output PCL on ABL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // output PCH on ABH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // increment PC for next T0
            o_i_pc = 1;

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

            // load Z and N from DB
            o_dbz_z = 1;
            o_db7_n = 1;

            // end of opcode
            o_tcu = 0;
        end
        AND_i, EOR_i, ORA_i, ADC_i, SBC_i,
        CMP_i, CPX_i, CPY_i:
        begin
            // output PCL on ABL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // output PCH on ABH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // increment PC for next T0
            o_i_pc = 1;

            // load immediate operand into ALU via DB from DL
            o_dl_db = 1;

            // load register into ALU via SB
            case (w_ir)
            CPX_i: o_x_sb = 1;
            CPY_i: o_y_sb = 1;
            default: o_ac_sb = 1;
            endcase
                
            o_sb_add = 1;

            case (w_ir)
            AND_i: begin
                o_db_add = 1;
                o_ands = 1;
            end
            EOR_i: begin
                o_db_add = 1;
                o_eors = 1;
            end
            ORA_i: begin
                o_db_add = 1;
                o_ors = 1;
            end
            ADC_i: begin
                o_db_add = 1;
                o_sums = 1;

                // carry in
                o_1_addc = i_p[C];

                // C + V flags
                o_acr_c = 1;
                o_avr_v = 1;
            end
            SBC_i: begin
                o_db_n_add = 1;
                o_sums = 1;
                
                // carry in
                o_1_addc = ~i_p[C];

                // C + V flags
                o_acr_c = 1;
                o_avr_v = 1;
            end
            CMP_i, CPX_i, CPY_i: begin
                // subtraction as 2's complement addition
                o_db_n_add = 1;
                o_1_addc = 1;

                o_sums = 1;

                // C + V flags
                o_acr_c = 1;
                o_avr_v = 1;
            end
            default: begin
            end
            endcase

            // end of opcode
            o_tcu = 0;
        end
        LDA_a, LDX_a, LDY_a,
        STA_a, STX_a, STY_a,
        BIT_a, JMP_a, JSR_a,
        INC_a, DEC_a, JMP_indirect:
        begin
            // PC + 1 = Fetch low order effective address byte

            // output PCL on ABL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // output PCH on ABH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // increment PC for T5
            o_i_pc = 1;

            // read low byte of address at end of phi 2
            // into ALU
            o_dl_db = 1;
            o_db_add = 1;
            o_0_add = 1;
            o_sums = 1;
        end
        RTS: begin
            // output PCL on ABL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // output PCH on ABH
            o_pch_adh = 1;
            o_adh_abh = 1;
        end
        TAX, TAY, TXA, TYA, TXS, TSX: begin
            // high byte - from PCH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // low byte - from PCL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

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
                o_dbz_z = 1;
                o_db7_n = 1;
            end
            
            // start next opcode 
            o_tcu = 0;
        end
        NOP:
        begin
            // high byte - from PCH
            o_pch_adh = 1;
            o_adh_abh = 1; 

            // low byte - from PCL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // next opcode
            o_tcu = 0;
        end
        BCC, BCS, BEQ, BNE, BMI, BPL, BVC, BVS: 
        begin
            // high byte - from PCH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // low byte - from PCL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            if ( ((i_p[C] == 0) && (w_ir == BCC)) ||
                 ((i_p[C] == 1) && (w_ir == BCS)) ||
                 ((i_p[Z] == 1) && (w_ir == BEQ)) ||
                 ((i_p[Z] == 0) && (w_ir == BNE)) ||
                 ((i_p[N] == 1) && (w_ir == BMI)) ||
                 ((i_p[N] == 0) && (w_ir == BPL)) ||
                 ((i_p[V] == 0) && (w_ir == BVC)) ||
                 ((i_p[V] == 1) && (w_ir == BVS)) )
            begin
                // use ALU to add offset to PC
                o_sums = 1;

                // PC + 1
                o_adl_add = 1;

                // + 1
                o_1_addc = 1;

                // + segment
                o_dl_db = 1;
                o_sb_db = 1;
                o_sb_add = 1;
            end
            else
            begin
                // increment PC for T0 next instruction
                o_i_pc = 1;

                // start next instruction
                o_tcu = 0;
            end
        end
        default:
        begin
            
        end
        endcase
    end
    2: // T2
    begin
        case (w_ir)
        BCC, BCS, BEQ, BNE, BMI, BPL, BVC, BVS:
        begin
            // high byte - from PCH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // low byte - from ALU
            o_add_adl = 1;
            o_adl_abl = 1;

            // retain PCH
            o_pch_pch = 1;

            // load PCL from ALU
            o_adl_pcl = 1;

            if (r_last_acr == 1)
            begin
                // use ALU to add 1 to ADH
                o_sums = 1;

                // B INPUT = pch
                o_pch_db = 1;
                o_db_add = 1;

                // + 1
                o_1_addc = 1;

                // A INPUT = 0
                o_0_add = 1;
            end
            else
            begin
                // start next instruction
                o_tcu = 0;
            end
        end
        JSR_a:
        begin
            // retain PCL and PCH (PC + 2)
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // output S on ABL
            o_s_adl = 1;
            o_adl_abl = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // load SP into ADD
            o_adl_add = 1;
            o_sums = 1;
            o_0_add = 1;

            // load ADD (subroutine address lo) into SP
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_s = 1;
        end
        RTS, RTI:
        begin
            // output S on ABL
            o_s_adl = 1;
            o_adl_abl = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // load SP + 1 into ADD
            o_adl_add = 1;
            o_1_addc = 1;
            o_sums = 1;
            o_0_add = 1;
        end
        BRK, PHA, PHP:
        begin
            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // output S on ABL
            o_s_adl = 1;
            o_adl_abl = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // use ALU to decrement the SP
            o_adl_add = 1;
            o_sb_add = 1;       // pre-charge mosfets = -1
            o_sums = 1;

            if (w_ir == PHA)
            begin
                o_rw = RW_WRITE;
                o_tcu = 0;      // start next opcode
                o_ac_db = 1;
            end
            else if (w_ir == PHP)
            begin
                o_rw = RW_WRITE;
                o_tcu = 0;      // start next opcode
                o_p_db = 1;
            end
            else if (w_ir == BRK)
            begin
                if (r_interrupt != INTERRUPT_RESET)
                begin
                    o_rw = RW_WRITE;
                    o_pch_db = 1;
                end
            end
        end
        PLA, PLP: begin
            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // output S on ABL
            o_s_adl = 1;
            o_adl_abl = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // write SP+1 to S
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_s = 1;

            // retain the value in ADD
            o_sb_add = 1;
            o_db_n_add = 1;
            o_sums = 1;
        end
        LDA_a, LDX_a, LDY_a,
        STA_a, STX_a, STY_a,
        BIT_a, INC_a, DEC_a:
        begin
            // PC + 2 = Fetch high order effective address byte
            
            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // output PCL on ABL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // output PCH on ABH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // keep value cached in ADD
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_add = 1;
            o_db_n_add = 1;      // inverse of mosfets
            o_sums = 1;
        end
        JMP_a, JMP_indirect:
        begin
            // PC + 2 = Fetch high order effective address byte

            if (r_clk == 0)
            begin
                // phase 1

                // output PCL on ABL
                o_pcl_adl = 1;
                o_adl_abl = 1;

                // output PCH on ABH
                o_pch_adh = 1;
                o_adh_abh = 1;
            end
            else
            begin
                // phase 2

                // load PCL from ALU
                o_add_adl = 1;
                o_adl_pcl = 1;

                // load PCH from Data bus
                o_dl_adh = 1;
                o_adh_pch = 1;
            end

            if (w_ir == JMP_a)
            begin
                o_tcu = 0;
            end
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
            // high byte - from ALU
            o_pch_adh = 1;
            o_adh_abh = 1;

            // low byte - from PCL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // retain PCL
            o_pcl_pcl = 1;

            // load PCH from ALU
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_adh = 1;
            o_adh_pch = 1;
        
            // start next instruction
            o_tcu = 0;
        end
        BRK:
        begin
            // output S-1 via ADL on ABL
            o_add_adl = 1;
            o_adl_abl = 1;

            // output 0x1 via ADH on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // use ALU to decrement the SP
            o_adl_add = 1;
            o_sb_add = 1;       // pre-charge mosfets = -1
            o_sums = 1;

            if (r_interrupt != INTERRUPT_RESET)
            begin
                o_rw = RW_WRITE;
                o_pcl_db = 1;
            end
        end
        LDA_a, LDX_a, LDY_a,
        STA_a, STX_a, STY_a,
        BIT_a, INC_a, DEC_a:
        begin
            // output absolute address ADH, ADL

            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // increment PC for next T0
            o_i_pc = 1;

            // output ADL from ADD on ABL
            o_add_adl = 1;
            o_adl_abl = 1;

            // output ADH from DL on ABH
            o_dl_adh = 1;
            o_adh_abh = 1;

            case (w_ir)
            INC_a, DEC_a: begin
                // load value from DL into ALU
                o_dl_db = 1;
                o_db_add = 1;
                o_sums = 1;
                o_0_add = 1;
            end
            LDA_a, LDX_a, LDY_a: begin
                // load value from DL into SB via DB
                o_dl_db = 1;
                o_sb_db = 1;

                case (w_ir)
                LDA_a: o_sb_ac = 1;
                LDX_a: o_sb_x = 1;
                LDY_a: o_sb_y = 1;
                default: begin
                end
                endcase

                // load Z and N from DB
                o_dbz_z = 1;
                o_db7_n = 1;
            end
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
                // load value from DL into ALU via DB
                o_dl_db = 1;
                o_db_add = 1;

                // load value from AC into ALU
                o_ac_sb = 1;
                o_sb_add = 1;

                // use ALU for AND
                o_ands = 1;

                // load V and N from DB
                o_db6_v = 1;
                o_db7_n = 1;
            end
            default: begin
            end
            endcase

            case (w_ir)
            INC_a, DEC_a: begin
            end
            default: begin
                // start next opcode
                o_tcu = 0;
            end
            endcase
        end
        PLA, PLP: begin
            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // output SP+1 on ABL
            o_add_adl = 1;
            o_adl_abl = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            o_tcu = 0;      // start next opcode

            if (w_ir == PLA) begin
                // read DL into AC
                o_dl_db = 1;
                
                o_sb_db = 1;
                o_sb_ac = 1;
            end
            else if (w_ir == PLP) begin
                // read DL into P
                o_dl_db = 1;

                o_db0_c = 1;
                o_db1_z = 1;
                o_db2_i = 1;
                o_db3_d = 1;
                o_db4_b = 1;
                o_db6_v = 1;
                o_db7_n = 1;
            end
        end
        JSR_a:
        begin
            // retain PCL and PCH (PC + 2)
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // output S on ABL from ALU
            o_add_adl = 1;
            o_adl_abl = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // write PCH to data bus
            o_rw = RW_WRITE;
            o_pch_db = 1;

            // decrement SP with ALU
            o_adl_add = 1;
            o_sums = 1;
            o_sb_add = 1;       // 0xff == -1
        end
        RTS, RTI:
        begin
            // output S+1 on ABL from ALU
            o_add_adl = 1;
            o_adl_abl = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // increment SP with ALU
            o_adl_add = 1;
            o_1_addc = 1;
            o_sums = 1;
            o_0_add = 1;

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
                // load P from stack
                o_db0_c = 1;
                o_db1_z = 1;
                o_db2_i = 1;
                o_db3_d = 1;
                o_db4_b = 1;
                o_db6_v = 1;
                o_db7_n = 1;
            end
            default: begin
            end
            endcase
        end
        JMP_indirect:
        begin
            // Indirect address = Fetch low order effective address byte

            // output PCL on ABL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // output PCH on ABH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // increment pc
            o_i_pc = 1;

            // read low byte of address at end of phi 2
            // into ALU
            o_dl_db = 1;
            o_db_add = 1;
            o_0_add = 1;
            o_sums = 1;
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
            // output s-2 on ABL
            o_add_adl = 1;
            o_adl_abl = 1;

            // use ALU to decrement the SP
            o_adl_add = 1;
            o_sb_add = 1;       // pre-charge mosfets = -1
            o_sums = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            if (r_interrupt != INTERRUPT_RESET)
            begin
                o_rw = RW_WRITE;
                o_p_db = 1;
            end
        end
        JSR_a:
        begin
            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // output S-1 on ABL from ALU
            o_add_adl = 1;
            o_adl_abl = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // write PCL to data bus
            o_rw = RW_WRITE;
            o_pcl_db = 1;

            // decrement SP with ALU
            o_adl_add = 1;
            o_sums = 1;
            o_sb_add = 1;       // 0xff == -1
        end
        RTS:
        begin
            if (r_clk == 0)
            begin
                // phi 1
            
                // output S+2 on ABL from ALU
                o_add_adl = 1;
                o_adl_abl = 1;

                // output 0x1 on ABH
                o_0_adh1_7 = 1;
                o_adh_abh = 1;
            end
            else
            begin
                // phi 2

                // load return address (lo) into PCL from SP
                o_s_adl = 1;
                o_adl_pcl = 1;

                // load return address (hi) into PCH
                o_dl_adh = 1;
                o_adh_pch = 1;
            end

            // read S+2 into SP from ALU
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_s = 1; 
        end
        RTI:
        begin
            // output S+2 on ABL from ALU
            o_add_adl = 1;
            o_adl_abl = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // increment SP with ALU
            o_adl_add = 1;
            o_1_addc = 1;
            o_sums = 1;
            o_0_add = 1;

            // load PCL into SP temporarily    
            o_dl_db = 1;
            o_sb_db = 1;
            o_sb_s = 1;
        end
        INC_a, DEC_a: 
        begin
            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // note: the correct address is already buffered in ABL/ABH

            // write from ALU
            o_rw = RW_WRITE;
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_db = 1;

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
                o_sb_add = 1;
                o_sums = 1;
                o_adl_add = 1;  // 0xFF from precharge mosfets
            end
            default:
            begin
            end
            endcase
        end
        JMP_indirect:
        begin
            // indirect address high - load high order byte of jump address

            if (r_clk == 0)
            begin
                // phase 1

                // output PCL on ABL
                o_pcl_adl = 1;
                o_adl_abl = 1;

                // output PCH on ABH
                o_pch_adh = 1;
                o_adh_abh = 1;
            end
            else
            begin
                // phase 2

                // load PCL from ALU
                o_add_adl = 1;
                o_adl_pcl = 1;

                // load PCH from Data bus
                o_dl_adh = 1;
                o_adh_pch = 1;
            end

            o_tcu = 0;
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
            /// @note currently hardcoded for RESET interrupt
            
            // load sp - 3 into stack register
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_s = 1;

            // >> address of interrupt vector low byte

            // ABH = 0xff
            o_adh_abh = 1;

            // ABL
            case (r_interrupt)
            INTERRUPT_RESET:
            begin
                // ABL = 0xfc
                o_0_adl0 = 1;
                o_0_adl1 = 1;
            end
            default:
            begin
                // ABL = 0xFE
                o_0_adl0 = 1;
            end
            endcase

            o_adl_abl = 1;

            // >> read low byte of reset vector at end of phi 2
            //  into ALU
            o_dl_db = 1;
            o_db_add = 1;
            o_0_add = 1;
            o_sums = 1;
        end
        RTI:
        begin
            if (r_clk == 0)
            begin
                // phi 1
            
                // output S+3 on ABL from ALU
                o_add_adl = 1;
                o_adl_abl = 1;

                // output 0x1 on ABH
                o_0_adh1_7 = 1;
                o_adh_abh = 1;
            end
            else
            begin
                // phi 2

                // load return address (lo) into PCL from SP
                o_s_adl = 1;
                o_adl_pcl = 1;

                // load return address (hi) into PCH
                o_dl_adh = 1;
                o_adh_pch = 1;
            end

            // read S+3 into SP from ALU
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_s = 1; 

            // start next opcode
            o_tcu = 0;
        end
        JSR_a:
        begin
            if (r_clk == 0)
            begin
                // phi 1 - output PC + 2 on address

                // retain PCL & PCH
                o_pcl_pcl = 1;
                o_pch_pch = 1;
                
                // output PCL on ABL
                o_pcl_adl = 1;
                o_adl_abl = 1;

                // output PCH on ABH
                o_pch_adh = 1;
                o_adh_abh = 1;    
            end
            else
            begin
                // phi 2

                // load ABL from cached lo (S?)
                o_s_adl = 1;
                o_adl_pcl = 1;

                // load PCH with ADH from DL
                o_dl_adh = 1;
                o_adh_pch = 1;

                // read sp-2 from ALU into Stack
                o_add_sb_0_6 = 1;
                o_add_sb_7 = 1;
                o_sb_s = 1;

            end

            // start next opcode
            o_tcu = 0;
        end
        RTS:
        begin
            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // output PCL on ABL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // output PCH on ABH
            o_pch_adh = 1;
            o_adh_abh = 1;

            // increment PC
            o_i_pc = 1;

            // start next opcode
            o_tcu = 0;
        end
        INC_a, DEC_a:
        begin
            // retain PCL and PCH
            o_pcl_pcl = 1;
            o_pch_pch = 1;

            // note: the correct address is already buffered in ABL/ABH

            // write from ALU
            o_rw = RW_WRITE;
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_db = 1;

            // load Z and N from DB
            o_dbz_z = 1;
            o_db7_n = 1;

            // start next opcode
            o_tcu = 0;
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
            /// @note currently hardcoded for RESET interrupt

            // >> setting up high byte of reset vector
            
            if (r_clk == 0)
            begin
                // phase 1

                // ABH = 0xff (precharge mosfets)
                o_adh_abh = 1;

                case (r_interrupt)
                INTERRUPT_RESET:
                begin
                    // ABL = 0xfd
                    o_0_adl1 = 1;
                end
                default:
                begin
                    // ABL = 0xFF
                    // (use precharge mosfets without modification)
                end
                endcase
                
                o_adl_abl = 1;
            end
            else
            begin
                // phase 2

                // >> read high byte of reset vector at end of phi 2
                o_dl_adh = 1;
                o_adh_pch = 1;     

                // >> read low byte of reset into PCL from ADD
                o_add_adl = 1;
                o_adl_pcl = 1;
            end

            if (r_interrupt == INTERRUPT_NONE)
            begin
                // using a BRK instruction
                o_db4_b = 1;
            end

            o_tcu = 0;
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
