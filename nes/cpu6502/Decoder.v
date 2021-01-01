// Model the Decode Rom in 6502
// -> combinatorial logic mapping (IR, TCU) => control lines
//
// IDEA: o_tcu_next, as a way to control when instructions
//       have finished and should return to T0/T1

module Decoder(
    /* verilator lint_off UNUSED */
    input [7:0] i_ir,
    input [2:0] i_tcu,
    /* verilator lint_on UNUSED */

    output reg [2:0] o_tcu,       // next value for tcu

    /* verilator lint_off UNDRIVEN */
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
    output reg o_adh_abh

    /* verilator lint_on UNDRIVEN */
);

always @(*)
begin
    o_tcu = i_tcu + 1;
end

endmodule
