// Model the Decode Rom in 6502
// -> combinatorial logic mapping (IR, TCU) => control lines
//
// IDEA: o_tcu_next, as a way to control when instructions
//       have finished and should return to T0/T1

module Decoder(
    input [7:0] i_ir,
    input [2:0] i_tcu,

    output reg [2:0] o_tcu,         // value TCU at next phi2 clock tick
    output reg o_interrupt,         // HIGH when interrupt is running

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

localparam [7:0] OPCODE_BRK = 8'h00;

localparam RW_READ = 1;
localparam RW_WRITE = 0;

always @(*)
begin
    // default TCU to increment at next clock tick
    o_tcu = i_tcu + 1;

    // default all control signals
    o_interrupt = 0;
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
        // increment PC for T1
        o_i_pc = 1;


        // fetch

        // output PCL on ABL
        o_pcl_adl = 1;
        o_adl_abl = 1;

        // output PCH on ABH
        o_pch_adh = 1;
        o_adh_abh = 1;

        case (i_ir)
        OPCODE_BRK:
        begin
            // during an interrupt, we force IR to use BRK opcode
            o_interrupt = 1;
        end
        default:
        begin
            
        end
        endcase
    end
    1: // T1
    begin
        case (i_ir)
        OPCODE_BRK: 
        begin
            // during an interrupt, we force IR to use BRK opcode
            o_interrupt = 1;

            // output PCL on ABL
            o_pcl_adl = 1;
            o_adl_abl = 1;

            // output PCH on ABH
            o_pch_adh = 1;
            o_adh_abh = 1;
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
            // during an interrupt, we force IR to use BRK opcode
            o_interrupt = 1;

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
            // output S-1 on ABL
            o_add_adl = 1;
            o_adl_abl = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // use ALU to decrement the SP
            o_adl_add = 1;
            o_sb_add = 1;       // pre-charge mosfets = -1
            o_sums = 1;
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
            // todo: should we be putting sp - 3 into stack register?
            o_add_sb_0_6 = 1;
            o_add_sb_7 = 1;
            o_sb_s = 1;

            // output 0x1 on ABH
            o_0_adh1_7 = 1;
            o_adh_abh = 1;

            // todo: decrement stack ptr for next tick
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
            // hardcoded for RESET interrupt
            
            // ABH = 0xff
            o_adh_abh = 1;

            // ABL = 0xfc
            o_adl_abl = 1;
            o_0_adl0 = 1;
            o_0_adl1 = 1;
        end
        default:
        begin
            
        end
        endcase
    end
    6: // T5
    begin
        case (i_ir)
        OPCODE_BRK:
        begin
            // hardcoded for RESET interrupt
            
            // ABH = 0xff
            o_adh_abh = 1;

            // ABL = 0xfd
            o_adl_abl = 1;
            o_0_adl1 = 1;

            // load address low -> where to?
            
        end
        default:
        begin
            // load address high
            
            
        end
        endcase
    end
    7: // T5
    begin
        
    end
    default:
    begin
        // unknown TCU state
    end
    endcase
end

endmodule
