// Model the Decode Rom in 6502
// -> combinatorial logic mapping (IR, TCU) => control lines
//
// IDEA: o_tcu_next, as a way to control when instructions
//       have finished and should return to T0/T1

module Decoder(
    input i_clk,
    
    input [7:0] i_ir,
    input [3:0] i_tcu,

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
    output reg o_srs
);

localparam [7:0] OPCODE_BRK = 8'h00, OPCODE_NOP = 8'hEA,
                 OPCODE_LDAi = 8'hA9, OPCODE_LDAa = 8'hAD;

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
    end
    1: // T1
    begin
        case (i_ir)
        OPCODE_BRK: 
        begin
            // output PCL on ABL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // output PCH on ABH
            o_pch_adh = 1;
            o_adh_abh = 1;
        end
        OPCODE_LDAi:
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

            // load Accumulator
            o_dl_db = 1;
            o_sb_db = 1;
            o_sb_ac = 1;

            // end of opcode
            o_tcu = 0;
        end
        OPCODE_LDAa:
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
        OPCODE_NOP:
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
        OPCODE_BRK:
        begin
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
        end
        OPCODE_LDAa:
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
            o_add_sb_0_6 = 1;    // TODO: test using o_add_sb_0_6 and 7 at the same time
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
        OPCODE_BRK:
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
        OPCODE_LDAa:
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

            // load value into AC
            o_dl_db = 1;
            o_sb_db = 1;
            o_sb_ac = 1;

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
        OPCODE_BRK:
        begin
            // output s-2 on ABL
            o_add_adl = 1;
            o_adl_abl = 1;

            // load sp - 2 into stack register
            /// @todo: should we be putting sp - 3 into stack register?
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_s = 1;

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
        OPCODE_BRK:
        begin
            /// @note currently hardcoded for RESET interrupt
            
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
        OPCODE_BRK:
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
