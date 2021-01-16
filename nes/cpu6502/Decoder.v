// Model the Decode Rom in 6502
// -> combinatorial logic mapping (IR, TCU, P) => control lines
//

// TODO: use 'functions' to enapsulate repeated combinatorial flags
//       simplify code, and self-document! 

module Decoder(
    input i_clk,
    
    input [7:0] i_ir,               // Instruction Register
    input [3:0] i_tcu,              // Opcode timing
    input [7:0] i_p,                // Processor status register

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
    output reg o_ir5_i
);

// Processor Status Register bitfields
localparam C = 0;       // Carry Flag

// Opcodes
localparam [7:0] BRK = 8'h00, NOP = 8'hEA,
                 INX = 8'hE8, INY = 8'hC8,
                 DEX = 8'hCA, DEY = 8'h88,
                 LDAi = 8'hA9, LDAa = 8'hAD,
                 LDXi = 8'hA2, LDXa = 8'hAE,
                 LDYi = 8'hA0, LDYa = 8'hAC,
                 STAa = 8'h8D,
                 TAX = 8'hAA, TAY = 8'hA8,
                 TSX = 8'hBA, TXA = 8'h8A,
                 TXS = 8'h9A, TYA = 8'h98,
                 LSR_A = 8'h4A, ASL_A = 8'h0A,
                 CLC = 8'h18, SEC = 8'h38,
                 SEI = 8'h78, CLI = 8'h58,
                 ROL_A = 8'h2A, ROR_A = 8'h6A,
                 PHA = 8'h48, PHP = 8'h08,
                 PLA = 8'h68, PLP = 8'h28;

// RW pin
localparam RW_READ = 1;
localparam RW_WRITE = 0;

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

        case (i_ir)
        INX, INY, DEX, DEY,
        LSR_A, ASL_A, ROL_A, ROR_A: begin
            // output value from ALU to SB
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;

            // load from SB into register
            case (i_ir)
            INX, DEX: o_sb_x = 1;
            INY, DEY: o_sb_y = 1;
            LSR_A, ASL_A, ROL_A, ROR_A: o_sb_ac = 1;
            default: begin
            end
            endcase
            
            // load Z & N status flags from SB via DB
            o_sb_db = 1;
            o_db7_n = 1;
            o_dbz_z = 1;
        end
        PHA, PLA, PLP: begin
            // store modified SP from ALU in S
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;

            o_sb_s = 1;
        end
        default: begin
        end
        endcase
    end
    1: // T1
    begin
        case (i_ir)
        BRK, PHA, PLA, PLP: 
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
        end
        SEC, CLC, SEI, CLI:
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

            case (i_ir)
            SEC, CLC: o_ir5_c = 1; // read C from IR5
            SEI, CLI: o_ir5_i = 1; // read I from IR5
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
            case (i_ir)
            INX, DEX: o_x_sb = 1;
            INY, DEY: o_y_sb = 1;
            LSR_A, ASL_A, ROL_A, ROR_A: o_ac_sb = 1;
            default: begin
            end
            endcase

            o_sb_add = 1;

            case (i_ir)
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

            case (i_ir)
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
        LDAi, LDXi, LDYi:
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

            case (i_ir)
            LDAi: o_sb_ac = 1;
            LDXi: o_sb_x = 1;
            LDYi: o_sb_y = 1;
            default: begin
            end
            endcase

            // load Z and N from DB
            o_dbz_z = 1;
            o_db7_n = 1;

            // end of opcode
            o_tcu = 0;
        end
        LDAa, LDXa, LDYa,
        STAa:
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

            // increment PC for T2
            o_i_pc = 1;

            // read low byte of address at end of phi 2
            // into ALU
            o_dl_db = 1;
            o_db_add = 1;
            o_0_add = 1;
            o_sums = 1;
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

            case (i_ir)
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
            if (i_ir != TXS)
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
        default:
        begin
            
        end
        endcase
    end
    2: // T2
    begin
        case (i_ir)
        BRK, PHA:
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

            if (i_ir == PHA)
            begin
                o_rw = RW_WRITE;
                o_tcu = 0;      // start next opcode

                o_ac_db = 1;    // output AC on DB
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

            // use ALU to increment the SP
            o_adl_add = 1;
            o_0_add = 1;
            o_1_addc = 1;
            o_sums = 1;

            o_tcu = 0;      // start next opcode

            if (i_ir == PLA) begin
                // read DL into AC
                o_dl_db = 1;
                o_sb_db = 1;
                o_sb_ac = 1;
            end
            else if (i_ir == PLP) begin
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
        LDAa, LDXa, LDYa,
        STAa:
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
        default:
        begin
        end
        endcase
    end
    3: // T3
    begin
        case (i_ir)
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
        end
        LDAa, LDXa, LDYa,
        STAa:
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

            case (i_ir)
            LDAa, LDXa, LDYa: begin
                // load value from DL into SB via DB
                o_dl_db = 1;
                o_sb_db = 1;

                case (i_ir)
                LDAa: o_sb_ac = 1;
                LDXa: o_sb_x = 1;
                LDYa: o_sb_y = 1;
                default: begin
                end
                endcase

                // load Z and N from DB
                o_dbz_z = 1;
                o_db7_n = 1;
            end
            STAa: begin
                // write value from AC
                o_ac_db = 1;
                o_rw = RW_WRITE;
            end
            default: begin
            end
            endcase

            // start next opcode
            o_tcu = 0;
        end
        default:
        begin
        end
        endcase
    end
    4: // T4
    begin
        case (i_ir)
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
        end
        default:
        begin
        end
        endcase
    end
    5: // T5
    begin
        case (i_ir)
        BRK:
        begin
            /// @note currently hardcoded for RESET interrupt
            
            // load sp - 3 into stack register
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_s = 1;

            // >> address of reset vector low byte

            // ABH = 0xff
            o_adh_abh = 1;

            // ABL = 0xfc
            o_adl_abl = 1;
            o_0_adl0 = 1;
            o_0_adl1 = 1;

            // >> read low byte of reset vector at end of phi 2
            //  into ALU
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
    6: // T6
    begin
        case (i_ir)
        BRK:
        begin
            /// @note currently hardcoded for RESET interrupt

            // >> setting up high byte of reset vector
            
            if (i_clk == 0)
            begin
                // phase 1

                // ABH = 0xff (precharge mosfets)
                o_adh_abh = 1;

                // ABL = 0xfd
                o_0_adl1 = 1;
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

            o_tcu = 0;
        end
        default:
        begin
            
        end
        endcase
    end
    7: // T7
    begin
        case (i_ir)
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
