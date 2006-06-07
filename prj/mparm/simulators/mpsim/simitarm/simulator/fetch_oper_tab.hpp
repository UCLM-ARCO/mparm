/*************************************************************************
    Copyright (C) 2002,2003,2004 Wei Qin
    See file COPYING for more information.

    This program is free software; you can redistribute it and/or modify    
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*************************************************************************/
#ifdef _DUMP_DECODE
#include <iostream>
#endif
unsigned DEC_fetch_oper_branch_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "branch_" << std::endl;
#endif
  return 0;
}

unsigned DEC_fetch_oper_branch_link_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "branch_link_" << std::endl;
#endif
  return 1;
}

unsigned DEC_fetch_oper_dpi_imm_binop_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_and_" << std::endl;
#endif
  return 2;
}

unsigned DEC_fetch_oper_dpi_imm_binop_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_ands_" << std::endl;
#endif
  return 3;
}

unsigned DEC_fetch_oper_dpi_imm_binop_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_eor_" << std::endl;
#endif
  return 4;
}

unsigned DEC_fetch_oper_dpi_imm_binop_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_eors_" << std::endl;
#endif
  return 5;
}

unsigned DEC_fetch_oper_dpi_imm_binop_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_sub_" << std::endl;
#endif
  return 6;
}

unsigned DEC_fetch_oper_dpi_imm_binop_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_subs_" << std::endl;
#endif
  return 7;
}

unsigned DEC_fetch_oper_dpi_imm_binop_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_rsb_" << std::endl;
#endif
  return 8;
}

unsigned DEC_fetch_oper_dpi_imm_binop_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_rsbs_" << std::endl;
#endif
  return 9;
}

unsigned DEC_fetch_oper_dpi_imm_binop_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_add_" << std::endl;
#endif
  return 10;
}

unsigned DEC_fetch_oper_dpi_imm_binop_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_adds_" << std::endl;
#endif
  return 11;
}

unsigned DEC_fetch_oper_dpi_imm_binop_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_adc_" << std::endl;
#endif
  return 12;
}

unsigned DEC_fetch_oper_dpi_imm_binop_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_adcs_" << std::endl;
#endif
  return 13;
}

unsigned DEC_fetch_oper_dpi_imm_binop_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_sbc_" << std::endl;
#endif
  return 14;
}

unsigned DEC_fetch_oper_dpi_imm_binop_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_sbcs_" << std::endl;
#endif
  return 15;
}

unsigned DEC_fetch_oper_dpi_imm_binop_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_rsc_" << std::endl;
#endif
  return 16;
}

unsigned DEC_fetch_oper_dpi_imm_binop_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_rscs_" << std::endl;
#endif
  return 17;
}

unsigned DEC_fetch_oper_dpi_imm_binop_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_orr_" << std::endl;
#endif
  return 18;
}

unsigned DEC_fetch_oper_dpi_imm_binop_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_orrs_" << std::endl;
#endif
  return 19;
}

unsigned DEC_fetch_oper_dpi_imm_binop_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_bic_" << std::endl;
#endif
  return 20;
}

unsigned DEC_fetch_oper_dpi_imm_binop_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_binop_bics_" << std::endl;
#endif
  return 21;
}

unsigned DEC_fetch_oper_dpi_imm_unop_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_unop_mov_" << std::endl;
#endif
  return 22;
}

unsigned DEC_fetch_oper_dpi_imm_unop_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_unop_movs_" << std::endl;
#endif
  return 23;
}

unsigned DEC_fetch_oper_dpi_imm_unop_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_unop_mvn_" << std::endl;
#endif
  return 24;
}

unsigned DEC_fetch_oper_dpi_imm_unop_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_unop_mvns_" << std::endl;
#endif
  return 25;
}

unsigned DEC_fetch_oper_dpi_imm_tst_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_tst_tst_" << std::endl;
#endif
  return 26;
}

unsigned DEC_fetch_oper_dpi_imm_tst_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_tst_teq_" << std::endl;
#endif
  return 27;
}

unsigned DEC_fetch_oper_dpi_imm_tst_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_tst_cmp_" << std::endl;
#endif
  return 28;
}

unsigned DEC_fetch_oper_dpi_imm_tst_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_imm_tst_cmn_" << std::endl;
#endif
  return 29;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_and_" << std::endl;
#endif
  return 30;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_ands_" << std::endl;
#endif
  return 31;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_eor_" << std::endl;
#endif
  return 32;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_eors_" << std::endl;
#endif
  return 33;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_sub_" << std::endl;
#endif
  return 34;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_subs_" << std::endl;
#endif
  return 35;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_rsb_" << std::endl;
#endif
  return 36;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_rsbs_" << std::endl;
#endif
  return 37;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_add_" << std::endl;
#endif
  return 38;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_adds_" << std::endl;
#endif
  return 39;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_adc_" << std::endl;
#endif
  return 40;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_adcs_" << std::endl;
#endif
  return 41;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_sbc_" << std::endl;
#endif
  return 42;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_sbcs_" << std::endl;
#endif
  return 43;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_rsc_" << std::endl;
#endif
  return 44;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_rscs_" << std::endl;
#endif
  return 45;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_orr_" << std::endl;
#endif
  return 46;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_orrs_" << std::endl;
#endif
  return 47;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_bic_" << std::endl;
#endif
  return 48;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsl_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsl_bics_" << std::endl;
#endif
  return 49;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_and_" << std::endl;
#endif
  return 50;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_ands_" << std::endl;
#endif
  return 51;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_eor_" << std::endl;
#endif
  return 52;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_eors_" << std::endl;
#endif
  return 53;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_sub_" << std::endl;
#endif
  return 54;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_subs_" << std::endl;
#endif
  return 55;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_rsb_" << std::endl;
#endif
  return 56;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_rsbs_" << std::endl;
#endif
  return 57;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_add_" << std::endl;
#endif
  return 58;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_adds_" << std::endl;
#endif
  return 59;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_adc_" << std::endl;
#endif
  return 60;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_adcs_" << std::endl;
#endif
  return 61;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_sbc_" << std::endl;
#endif
  return 62;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_sbcs_" << std::endl;
#endif
  return 63;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_rsc_" << std::endl;
#endif
  return 64;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_rscs_" << std::endl;
#endif
  return 65;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_orr_" << std::endl;
#endif
  return 66;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_orrs_" << std::endl;
#endif
  return 67;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_bic_" << std::endl;
#endif
  return 68;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_lsr_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_lsr_bics_" << std::endl;
#endif
  return 69;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_and_" << std::endl;
#endif
  return 70;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_ands_" << std::endl;
#endif
  return 71;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_eor_" << std::endl;
#endif
  return 72;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_eors_" << std::endl;
#endif
  return 73;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_sub_" << std::endl;
#endif
  return 74;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_subs_" << std::endl;
#endif
  return 75;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_rsb_" << std::endl;
#endif
  return 76;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_rsbs_" << std::endl;
#endif
  return 77;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_add_" << std::endl;
#endif
  return 78;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_adds_" << std::endl;
#endif
  return 79;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_adc_" << std::endl;
#endif
  return 80;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_adcs_" << std::endl;
#endif
  return 81;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_sbc_" << std::endl;
#endif
  return 82;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_sbcs_" << std::endl;
#endif
  return 83;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_rsc_" << std::endl;
#endif
  return 84;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_rscs_" << std::endl;
#endif
  return 85;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_orr_" << std::endl;
#endif
  return 86;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_orrs_" << std::endl;
#endif
  return 87;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_bic_" << std::endl;
#endif
  return 88;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_asr_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_asr_bics_" << std::endl;
#endif
  return 89;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_and_" << std::endl;
#endif
  return 90;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_ands_" << std::endl;
#endif
  return 91;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_eor_" << std::endl;
#endif
  return 92;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_eors_" << std::endl;
#endif
  return 93;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_sub_" << std::endl;
#endif
  return 94;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_subs_" << std::endl;
#endif
  return 95;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_rsb_" << std::endl;
#endif
  return 96;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_rsbs_" << std::endl;
#endif
  return 97;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_add_" << std::endl;
#endif
  return 98;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_adds_" << std::endl;
#endif
  return 99;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_adc_" << std::endl;
#endif
  return 100;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_adcs_" << std::endl;
#endif
  return 101;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_sbc_" << std::endl;
#endif
  return 102;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_sbcs_" << std::endl;
#endif
  return 103;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_rsc_" << std::endl;
#endif
  return 104;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_rscs_" << std::endl;
#endif
  return 105;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_orr_" << std::endl;
#endif
  return 106;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_orrs_" << std::endl;
#endif
  return 107;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_bic_" << std::endl;
#endif
  return 108;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_zero_shift_zero_ror_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_zero_shift_zero_ror_bics_" << std::endl;
#endif
  return 109;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_and_" << std::endl;
#endif
  return 110;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_ands_" << std::endl;
#endif
  return 111;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_eor_" << std::endl;
#endif
  return 112;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_eors_" << std::endl;
#endif
  return 113;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_sub_" << std::endl;
#endif
  return 114;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_subs_" << std::endl;
#endif
  return 115;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_rsb_" << std::endl;
#endif
  return 116;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_rsbs_" << std::endl;
#endif
  return 117;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_add_" << std::endl;
#endif
  return 118;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_adds_" << std::endl;
#endif
  return 119;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_adc_" << std::endl;
#endif
  return 120;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_adcs_" << std::endl;
#endif
  return 121;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_sbc_" << std::endl;
#endif
  return 122;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_sbcs_" << std::endl;
#endif
  return 123;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_rsc_" << std::endl;
#endif
  return 124;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_rscs_" << std::endl;
#endif
  return 125;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_orr_" << std::endl;
#endif
  return 126;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_orrs_" << std::endl;
#endif
  return 127;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_bic_" << std::endl;
#endif
  return 128;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsl_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsl_bics_" << std::endl;
#endif
  return 129;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_and_" << std::endl;
#endif
  return 130;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_ands_" << std::endl;
#endif
  return 131;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_eor_" << std::endl;
#endif
  return 132;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_eors_" << std::endl;
#endif
  return 133;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_sub_" << std::endl;
#endif
  return 134;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_subs_" << std::endl;
#endif
  return 135;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_rsb_" << std::endl;
#endif
  return 136;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_rsbs_" << std::endl;
#endif
  return 137;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_add_" << std::endl;
#endif
  return 138;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_adds_" << std::endl;
#endif
  return 139;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_adc_" << std::endl;
#endif
  return 140;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_adcs_" << std::endl;
#endif
  return 141;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_sbc_" << std::endl;
#endif
  return 142;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_sbcs_" << std::endl;
#endif
  return 143;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_rsc_" << std::endl;
#endif
  return 144;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_rscs_" << std::endl;
#endif
  return 145;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_orr_" << std::endl;
#endif
  return 146;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_orrs_" << std::endl;
#endif
  return 147;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_bic_" << std::endl;
#endif
  return 148;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_lsr_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_lsr_bics_" << std::endl;
#endif
  return 149;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_and_" << std::endl;
#endif
  return 150;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_ands_" << std::endl;
#endif
  return 151;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_eor_" << std::endl;
#endif
  return 152;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_eors_" << std::endl;
#endif
  return 153;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_sub_" << std::endl;
#endif
  return 154;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_subs_" << std::endl;
#endif
  return 155;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_rsb_" << std::endl;
#endif
  return 156;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_rsbs_" << std::endl;
#endif
  return 157;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_add_" << std::endl;
#endif
  return 158;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_adds_" << std::endl;
#endif
  return 159;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_adc_" << std::endl;
#endif
  return 160;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_adcs_" << std::endl;
#endif
  return 161;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_sbc_" << std::endl;
#endif
  return 162;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_sbcs_" << std::endl;
#endif
  return 163;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_rsc_" << std::endl;
#endif
  return 164;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_rscs_" << std::endl;
#endif
  return 165;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_orr_" << std::endl;
#endif
  return 166;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_orrs_" << std::endl;
#endif
  return 167;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_bic_" << std::endl;
#endif
  return 168;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_asr_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_asr_bics_" << std::endl;
#endif
  return 169;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_and_" << std::endl;
#endif
  return 170;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_ands_" << std::endl;
#endif
  return 171;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_eor_" << std::endl;
#endif
  return 172;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_eors_" << std::endl;
#endif
  return 173;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_sub_" << std::endl;
#endif
  return 174;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_subs_" << std::endl;
#endif
  return 175;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_rsb_" << std::endl;
#endif
  return 176;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_rsbs_" << std::endl;
#endif
  return 177;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_add_" << std::endl;
#endif
  return 178;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_adds_" << std::endl;
#endif
  return 179;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_adc_" << std::endl;
#endif
  return 180;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_adcs_" << std::endl;
#endif
  return 181;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_sbc_" << std::endl;
#endif
  return 182;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_sbcs_" << std::endl;
#endif
  return 183;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_rsc_" << std::endl;
#endif
  return 184;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_rscs_" << std::endl;
#endif
  return 185;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_orr_" << std::endl;
#endif
  return 186;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_orrs_" << std::endl;
#endif
  return 187;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_bic_" << std::endl;
#endif
  return 188;
}

unsigned DEC_fetch_oper_dpi_ishift_binop_imm_shift_i_ror_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_binop_imm_shift_i_ror_bics_" << std::endl;
#endif
  return 189;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_lsl_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_lsl_mov_" << std::endl;
#endif
  return 190;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_lsl_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_lsl_movs_" << std::endl;
#endif
  return 191;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_lsl_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_lsl_mvn_" << std::endl;
#endif
  return 192;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_lsl_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_lsl_mvns_" << std::endl;
#endif
  return 193;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_lsr_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_lsr_mov_" << std::endl;
#endif
  return 194;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_lsr_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_lsr_movs_" << std::endl;
#endif
  return 195;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_lsr_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_lsr_mvn_" << std::endl;
#endif
  return 196;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_lsr_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_lsr_mvns_" << std::endl;
#endif
  return 197;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_asr_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_asr_mov_" << std::endl;
#endif
  return 198;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_asr_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_asr_movs_" << std::endl;
#endif
  return 199;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_asr_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_asr_mvn_" << std::endl;
#endif
  return 200;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_asr_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_asr_mvns_" << std::endl;
#endif
  return 201;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_ror_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_ror_mov_" << std::endl;
#endif
  return 202;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_ror_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_ror_movs_" << std::endl;
#endif
  return 203;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_ror_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_ror_mvn_" << std::endl;
#endif
  return 204;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_zero_shift_zero_ror_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_zero_shift_zero_ror_mvns_" << std::endl;
#endif
  return 205;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_lsl_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_lsl_mov_" << std::endl;
#endif
  return 206;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_lsl_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_lsl_movs_" << std::endl;
#endif
  return 207;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_lsl_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_lsl_mvn_" << std::endl;
#endif
  return 208;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_lsl_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_lsl_mvns_" << std::endl;
#endif
  return 209;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_lsr_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_lsr_mov_" << std::endl;
#endif
  return 210;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_lsr_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_lsr_movs_" << std::endl;
#endif
  return 211;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_lsr_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_lsr_mvn_" << std::endl;
#endif
  return 212;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_lsr_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_lsr_mvns_" << std::endl;
#endif
  return 213;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_asr_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_asr_mov_" << std::endl;
#endif
  return 214;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_asr_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_asr_movs_" << std::endl;
#endif
  return 215;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_asr_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_asr_mvn_" << std::endl;
#endif
  return 216;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_asr_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_asr_mvns_" << std::endl;
#endif
  return 217;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_ror_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_ror_mov_" << std::endl;
#endif
  return 218;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_ror_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_ror_movs_" << std::endl;
#endif
  return 219;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_ror_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_ror_mvn_" << std::endl;
#endif
  return 220;
}

unsigned DEC_fetch_oper_dpi_ishift_unop_imm_shift_i_ror_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_unop_imm_shift_i_ror_mvns_" << std::endl;
#endif
  return 221;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_lsl_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_lsl_tst_" << std::endl;
#endif
  return 222;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_lsl_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_lsl_teq_" << std::endl;
#endif
  return 223;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_lsl_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_lsl_cmp_" << std::endl;
#endif
  return 224;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_lsl_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_lsl_cmn_" << std::endl;
#endif
  return 225;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_lsr_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_lsr_tst_" << std::endl;
#endif
  return 226;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_lsr_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_lsr_teq_" << std::endl;
#endif
  return 227;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_lsr_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_lsr_cmp_" << std::endl;
#endif
  return 228;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_lsr_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_lsr_cmn_" << std::endl;
#endif
  return 229;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_asr_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_asr_tst_" << std::endl;
#endif
  return 230;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_asr_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_asr_teq_" << std::endl;
#endif
  return 231;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_asr_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_asr_cmp_" << std::endl;
#endif
  return 232;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_asr_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_asr_cmn_" << std::endl;
#endif
  return 233;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_ror_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_ror_tst_" << std::endl;
#endif
  return 234;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_ror_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_ror_teq_" << std::endl;
#endif
  return 235;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_ror_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_ror_cmp_" << std::endl;
#endif
  return 236;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_zero_shift_zero_ror_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_zero_shift_zero_ror_cmn_" << std::endl;
#endif
  return 237;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_lsl_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_lsl_tst_" << std::endl;
#endif
  return 238;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_lsl_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_lsl_teq_" << std::endl;
#endif
  return 239;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_lsl_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_lsl_cmp_" << std::endl;
#endif
  return 240;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_lsl_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_lsl_cmn_" << std::endl;
#endif
  return 241;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_lsr_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_lsr_tst_" << std::endl;
#endif
  return 242;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_lsr_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_lsr_teq_" << std::endl;
#endif
  return 243;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_lsr_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_lsr_cmp_" << std::endl;
#endif
  return 244;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_lsr_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_lsr_cmn_" << std::endl;
#endif
  return 245;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_asr_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_asr_tst_" << std::endl;
#endif
  return 246;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_asr_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_asr_teq_" << std::endl;
#endif
  return 247;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_asr_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_asr_cmp_" << std::endl;
#endif
  return 248;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_asr_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_asr_cmn_" << std::endl;
#endif
  return 249;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_ror_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_ror_tst_" << std::endl;
#endif
  return 250;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_ror_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_ror_teq_" << std::endl;
#endif
  return 251;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_ror_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_ror_cmp_" << std::endl;
#endif
  return 252;
}

unsigned DEC_fetch_oper_dpi_ishift_tst_imm_shift_i_ror_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_ishift_tst_imm_shift_i_ror_cmn_" << std::endl;
#endif
  return 253;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_and_" << std::endl;
#endif
  return 254;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_ands_" << std::endl;
#endif
  return 255;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_eor_" << std::endl;
#endif
  return 256;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_eors_" << std::endl;
#endif
  return 257;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_sub_" << std::endl;
#endif
  return 258;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_subs_" << std::endl;
#endif
  return 259;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_rsb_" << std::endl;
#endif
  return 260;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_rsbs_" << std::endl;
#endif
  return 261;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_add_" << std::endl;
#endif
  return 262;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_adds_" << std::endl;
#endif
  return 263;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_adc_" << std::endl;
#endif
  return 264;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_adcs_" << std::endl;
#endif
  return 265;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_sbc_" << std::endl;
#endif
  return 266;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_sbcs_" << std::endl;
#endif
  return 267;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_rsc_" << std::endl;
#endif
  return 268;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_rscs_" << std::endl;
#endif
  return 269;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_orr_" << std::endl;
#endif
  return 270;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_orrs_" << std::endl;
#endif
  return 271;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_bic_" << std::endl;
#endif
  return 272;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsl_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsl_bics_" << std::endl;
#endif
  return 273;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_and_" << std::endl;
#endif
  return 274;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_ands_" << std::endl;
#endif
  return 275;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_eor_" << std::endl;
#endif
  return 276;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_eors_" << std::endl;
#endif
  return 277;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_sub_" << std::endl;
#endif
  return 278;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_subs_" << std::endl;
#endif
  return 279;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_rsb_" << std::endl;
#endif
  return 280;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_rsbs_" << std::endl;
#endif
  return 281;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_add_" << std::endl;
#endif
  return 282;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_adds_" << std::endl;
#endif
  return 283;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_adc_" << std::endl;
#endif
  return 284;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_adcs_" << std::endl;
#endif
  return 285;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_sbc_" << std::endl;
#endif
  return 286;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_sbcs_" << std::endl;
#endif
  return 287;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_rsc_" << std::endl;
#endif
  return 288;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_rscs_" << std::endl;
#endif
  return 289;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_orr_" << std::endl;
#endif
  return 290;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_orrs_" << std::endl;
#endif
  return 291;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_bic_" << std::endl;
#endif
  return 292;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_lsr_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_lsr_bics_" << std::endl;
#endif
  return 293;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_and_" << std::endl;
#endif
  return 294;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_ands_" << std::endl;
#endif
  return 295;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_eor_" << std::endl;
#endif
  return 296;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_eors_" << std::endl;
#endif
  return 297;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_sub_" << std::endl;
#endif
  return 298;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_subs_" << std::endl;
#endif
  return 299;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_rsb_" << std::endl;
#endif
  return 300;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_rsbs_" << std::endl;
#endif
  return 301;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_add_" << std::endl;
#endif
  return 302;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_adds_" << std::endl;
#endif
  return 303;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_adc_" << std::endl;
#endif
  return 304;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_adcs_" << std::endl;
#endif
  return 305;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_sbc_" << std::endl;
#endif
  return 306;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_sbcs_" << std::endl;
#endif
  return 307;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_rsc_" << std::endl;
#endif
  return 308;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_rscs_" << std::endl;
#endif
  return 309;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_orr_" << std::endl;
#endif
  return 310;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_orrs_" << std::endl;
#endif
  return 311;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_bic_" << std::endl;
#endif
  return 312;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_asr_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_asr_bics_" << std::endl;
#endif
  return 313;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_and_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_and_" << std::endl;
#endif
  return 314;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_ands_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_ands_" << std::endl;
#endif
  return 315;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_eor_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_eor_" << std::endl;
#endif
  return 316;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_eors_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_eors_" << std::endl;
#endif
  return 317;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_sub_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_sub_" << std::endl;
#endif
  return 318;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_subs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_subs_" << std::endl;
#endif
  return 319;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_rsb_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_rsb_" << std::endl;
#endif
  return 320;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_rsbs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_rsbs_" << std::endl;
#endif
  return 321;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_add_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_add_" << std::endl;
#endif
  return 322;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_adds_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_adds_" << std::endl;
#endif
  return 323;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_adc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_adc_" << std::endl;
#endif
  return 324;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_adcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_adcs_" << std::endl;
#endif
  return 325;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_sbc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_sbc_" << std::endl;
#endif
  return 326;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_sbcs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_sbcs_" << std::endl;
#endif
  return 327;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_rsc_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_rsc_" << std::endl;
#endif
  return 328;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_rscs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_rscs_" << std::endl;
#endif
  return 329;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_orr_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_orr_" << std::endl;
#endif
  return 330;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_orrs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_orrs_" << std::endl;
#endif
  return 331;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_bic_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_bic_" << std::endl;
#endif
  return 332;
}

unsigned DEC_fetch_oper_dpi_rshift_binop_r_ror_bics_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_binop_r_ror_bics_" << std::endl;
#endif
  return 333;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_lsl_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_lsl_mov_" << std::endl;
#endif
  return 334;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_lsl_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_lsl_movs_" << std::endl;
#endif
  return 335;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_lsl_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_lsl_mvn_" << std::endl;
#endif
  return 336;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_lsl_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_lsl_mvns_" << std::endl;
#endif
  return 337;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_lsr_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_lsr_mov_" << std::endl;
#endif
  return 338;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_lsr_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_lsr_movs_" << std::endl;
#endif
  return 339;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_lsr_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_lsr_mvn_" << std::endl;
#endif
  return 340;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_lsr_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_lsr_mvns_" << std::endl;
#endif
  return 341;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_asr_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_asr_mov_" << std::endl;
#endif
  return 342;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_asr_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_asr_movs_" << std::endl;
#endif
  return 343;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_asr_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_asr_mvn_" << std::endl;
#endif
  return 344;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_asr_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_asr_mvns_" << std::endl;
#endif
  return 345;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_ror_mov_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_ror_mov_" << std::endl;
#endif
  return 346;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_ror_movs_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_ror_movs_" << std::endl;
#endif
  return 347;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_ror_mvn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_ror_mvn_" << std::endl;
#endif
  return 348;
}

unsigned DEC_fetch_oper_dpi_rshift_unop_r_ror_mvns_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_unop_r_ror_mvns_" << std::endl;
#endif
  return 349;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_lsl_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_lsl_tst_" << std::endl;
#endif
  return 350;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_lsl_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_lsl_teq_" << std::endl;
#endif
  return 351;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_lsl_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_lsl_cmp_" << std::endl;
#endif
  return 352;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_lsl_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_lsl_cmn_" << std::endl;
#endif
  return 353;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_lsr_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_lsr_tst_" << std::endl;
#endif
  return 354;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_lsr_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_lsr_teq_" << std::endl;
#endif
  return 355;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_lsr_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_lsr_cmp_" << std::endl;
#endif
  return 356;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_lsr_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_lsr_cmn_" << std::endl;
#endif
  return 357;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_asr_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_asr_tst_" << std::endl;
#endif
  return 358;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_asr_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_asr_teq_" << std::endl;
#endif
  return 359;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_asr_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_asr_cmp_" << std::endl;
#endif
  return 360;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_asr_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_asr_cmn_" << std::endl;
#endif
  return 361;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_ror_tst_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_ror_tst_" << std::endl;
#endif
  return 362;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_ror_teq_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_ror_teq_" << std::endl;
#endif
  return 363;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_ror_cmp_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_ror_cmp_" << std::endl;
#endif
  return 364;
}

unsigned DEC_fetch_oper_dpi_rshift_tst_r_ror_cmn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "dpi_rshift_tst_r_ror_cmn_" << std::endl;
#endif
  return 365;
}

unsigned DEC_fetch_oper_load_imm_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_imm_mem_mode_0_" << std::endl;
#endif
  return 366;
}

unsigned DEC_fetch_oper_load_imm_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_imm_mem_mode_1_" << std::endl;
#endif
  return 367;
}

unsigned DEC_fetch_oper_load_imm_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_imm_mem_mode_2_" << std::endl;
#endif
  return 368;
}

unsigned DEC_fetch_oper_load_imm_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_imm_mem_mode_3_" << std::endl;
#endif
  return 369;
}

unsigned DEC_fetch_oper_load_imm_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_imm_mem_mode_4_" << std::endl;
#endif
  return 370;
}

unsigned DEC_fetch_oper_load_imm_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_imm_mem_mode_5_" << std::endl;
#endif
  return 371;
}

unsigned DEC_fetch_oper_load_imm_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_imm_mem_mode_6_" << std::endl;
#endif
  return 372;
}

unsigned DEC_fetch_oper_load_imm_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_imm_mem_mode_7_" << std::endl;
#endif
  return 373;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsl_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsl_mem_mode_0_" << std::endl;
#endif
  return 374;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsl_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsl_mem_mode_1_" << std::endl;
#endif
  return 375;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsl_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsl_mem_mode_2_" << std::endl;
#endif
  return 376;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsl_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsl_mem_mode_3_" << std::endl;
#endif
  return 377;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsl_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsl_mem_mode_4_" << std::endl;
#endif
  return 378;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsl_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsl_mem_mode_5_" << std::endl;
#endif
  return 379;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsl_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsl_mem_mode_6_" << std::endl;
#endif
  return 380;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsl_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsl_mem_mode_7_" << std::endl;
#endif
  return 381;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsr_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsr_mem_mode_0_" << std::endl;
#endif
  return 382;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsr_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsr_mem_mode_1_" << std::endl;
#endif
  return 383;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsr_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsr_mem_mode_2_" << std::endl;
#endif
  return 384;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsr_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsr_mem_mode_3_" << std::endl;
#endif
  return 385;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsr_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsr_mem_mode_4_" << std::endl;
#endif
  return 386;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsr_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsr_mem_mode_5_" << std::endl;
#endif
  return 387;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsr_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsr_mem_mode_6_" << std::endl;
#endif
  return 388;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_lsr_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_lsr_mem_mode_7_" << std::endl;
#endif
  return 389;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_asr_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_asr_mem_mode_0_" << std::endl;
#endif
  return 390;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_asr_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_asr_mem_mode_1_" << std::endl;
#endif
  return 391;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_asr_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_asr_mem_mode_2_" << std::endl;
#endif
  return 392;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_asr_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_asr_mem_mode_3_" << std::endl;
#endif
  return 393;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_asr_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_asr_mem_mode_4_" << std::endl;
#endif
  return 394;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_asr_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_asr_mem_mode_5_" << std::endl;
#endif
  return 395;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_asr_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_asr_mem_mode_6_" << std::endl;
#endif
  return 396;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_asr_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_asr_mem_mode_7_" << std::endl;
#endif
  return 397;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_ror_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_ror_mem_mode_0_" << std::endl;
#endif
  return 398;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_ror_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_ror_mem_mode_1_" << std::endl;
#endif
  return 399;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_ror_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_ror_mem_mode_2_" << std::endl;
#endif
  return 400;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_ror_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_ror_mem_mode_3_" << std::endl;
#endif
  return 401;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_ror_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_ror_mem_mode_4_" << std::endl;
#endif
  return 402;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_ror_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_ror_mem_mode_5_" << std::endl;
#endif
  return 403;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_ror_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_ror_mem_mode_6_" << std::endl;
#endif
  return 404;
}

unsigned DEC_fetch_oper_load_reg_zero_shift_zero_ror_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_zero_shift_zero_ror_mem_mode_7_" << std::endl;
#endif
  return 405;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsl_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsl_mem_mode_0_" << std::endl;
#endif
  return 406;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsl_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsl_mem_mode_1_" << std::endl;
#endif
  return 407;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsl_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsl_mem_mode_2_" << std::endl;
#endif
  return 408;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsl_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsl_mem_mode_3_" << std::endl;
#endif
  return 409;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsl_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsl_mem_mode_4_" << std::endl;
#endif
  return 410;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsl_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsl_mem_mode_5_" << std::endl;
#endif
  return 411;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsl_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsl_mem_mode_6_" << std::endl;
#endif
  return 412;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsl_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsl_mem_mode_7_" << std::endl;
#endif
  return 413;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsr_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsr_mem_mode_0_" << std::endl;
#endif
  return 414;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsr_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsr_mem_mode_1_" << std::endl;
#endif
  return 415;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsr_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsr_mem_mode_2_" << std::endl;
#endif
  return 416;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsr_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsr_mem_mode_3_" << std::endl;
#endif
  return 417;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsr_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsr_mem_mode_4_" << std::endl;
#endif
  return 418;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsr_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsr_mem_mode_5_" << std::endl;
#endif
  return 419;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsr_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsr_mem_mode_6_" << std::endl;
#endif
  return 420;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_lsr_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_lsr_mem_mode_7_" << std::endl;
#endif
  return 421;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_asr_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_asr_mem_mode_0_" << std::endl;
#endif
  return 422;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_asr_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_asr_mem_mode_1_" << std::endl;
#endif
  return 423;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_asr_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_asr_mem_mode_2_" << std::endl;
#endif
  return 424;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_asr_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_asr_mem_mode_3_" << std::endl;
#endif
  return 425;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_asr_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_asr_mem_mode_4_" << std::endl;
#endif
  return 426;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_asr_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_asr_mem_mode_5_" << std::endl;
#endif
  return 427;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_asr_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_asr_mem_mode_6_" << std::endl;
#endif
  return 428;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_asr_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_asr_mem_mode_7_" << std::endl;
#endif
  return 429;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_ror_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_ror_mem_mode_0_" << std::endl;
#endif
  return 430;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_ror_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_ror_mem_mode_1_" << std::endl;
#endif
  return 431;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_ror_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_ror_mem_mode_2_" << std::endl;
#endif
  return 432;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_ror_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_ror_mem_mode_3_" << std::endl;
#endif
  return 433;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_ror_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_ror_mem_mode_4_" << std::endl;
#endif
  return 434;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_ror_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_ror_mem_mode_5_" << std::endl;
#endif
  return 435;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_ror_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_ror_mem_mode_6_" << std::endl;
#endif
  return 436;
}

unsigned DEC_fetch_oper_load_reg_imm_shift_i_ror_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_reg_imm_shift_i_ror_mem_mode_7_" << std::endl;
#endif
  return 437;
}

unsigned DEC_fetch_oper_load_ext_imm_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_ext_imm_mem_mode_2_" << std::endl;
#endif
  return 438;
}

unsigned DEC_fetch_oper_load_ext_imm_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_ext_imm_mem_mode_3_" << std::endl;
#endif
  return 439;
}

unsigned DEC_fetch_oper_load_ext_imm_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_ext_imm_mem_mode_6_" << std::endl;
#endif
  return 440;
}

unsigned DEC_fetch_oper_load_ext_imm_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_ext_imm_mem_mode_7_" << std::endl;
#endif
  return 441;
}

unsigned DEC_fetch_oper_load_ext_reg_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_ext_reg_mem_mode_0_" << std::endl;
#endif
  return 442;
}

unsigned DEC_fetch_oper_load_ext_reg_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_ext_reg_mem_mode_1_" << std::endl;
#endif
  return 443;
}

unsigned DEC_fetch_oper_load_ext_reg_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_ext_reg_mem_mode_4_" << std::endl;
#endif
  return 444;
}

unsigned DEC_fetch_oper_load_ext_reg_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "load_ext_reg_mem_mode_5_" << std::endl;
#endif
  return 445;
}

unsigned DEC_fetch_oper_store_imm_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_imm_mem_mode_0_" << std::endl;
#endif
  return 446;
}

unsigned DEC_fetch_oper_store_imm_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_imm_mem_mode_1_" << std::endl;
#endif
  return 447;
}

unsigned DEC_fetch_oper_store_imm_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_imm_mem_mode_2_" << std::endl;
#endif
  return 448;
}

unsigned DEC_fetch_oper_store_imm_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_imm_mem_mode_3_" << std::endl;
#endif
  return 449;
}

unsigned DEC_fetch_oper_store_imm_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_imm_mem_mode_4_" << std::endl;
#endif
  return 450;
}

unsigned DEC_fetch_oper_store_imm_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_imm_mem_mode_5_" << std::endl;
#endif
  return 451;
}

unsigned DEC_fetch_oper_store_imm_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_imm_mem_mode_6_" << std::endl;
#endif
  return 452;
}

unsigned DEC_fetch_oper_store_imm_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_imm_mem_mode_7_" << std::endl;
#endif
  return 453;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsl_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsl_mem_mode_0_" << std::endl;
#endif
  return 454;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsl_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsl_mem_mode_1_" << std::endl;
#endif
  return 455;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsl_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsl_mem_mode_2_" << std::endl;
#endif
  return 456;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsl_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsl_mem_mode_3_" << std::endl;
#endif
  return 457;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsl_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsl_mem_mode_4_" << std::endl;
#endif
  return 458;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsl_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsl_mem_mode_5_" << std::endl;
#endif
  return 459;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsl_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsl_mem_mode_6_" << std::endl;
#endif
  return 460;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsl_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsl_mem_mode_7_" << std::endl;
#endif
  return 461;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsr_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsr_mem_mode_0_" << std::endl;
#endif
  return 462;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsr_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsr_mem_mode_1_" << std::endl;
#endif
  return 463;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsr_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsr_mem_mode_2_" << std::endl;
#endif
  return 464;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsr_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsr_mem_mode_3_" << std::endl;
#endif
  return 465;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsr_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsr_mem_mode_4_" << std::endl;
#endif
  return 466;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsr_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsr_mem_mode_5_" << std::endl;
#endif
  return 467;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsr_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsr_mem_mode_6_" << std::endl;
#endif
  return 468;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_lsr_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_lsr_mem_mode_7_" << std::endl;
#endif
  return 469;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_asr_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_asr_mem_mode_0_" << std::endl;
#endif
  return 470;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_asr_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_asr_mem_mode_1_" << std::endl;
#endif
  return 471;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_asr_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_asr_mem_mode_2_" << std::endl;
#endif
  return 472;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_asr_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_asr_mem_mode_3_" << std::endl;
#endif
  return 473;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_asr_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_asr_mem_mode_4_" << std::endl;
#endif
  return 474;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_asr_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_asr_mem_mode_5_" << std::endl;
#endif
  return 475;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_asr_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_asr_mem_mode_6_" << std::endl;
#endif
  return 476;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_asr_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_asr_mem_mode_7_" << std::endl;
#endif
  return 477;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_ror_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_ror_mem_mode_0_" << std::endl;
#endif
  return 478;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_ror_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_ror_mem_mode_1_" << std::endl;
#endif
  return 479;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_ror_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_ror_mem_mode_2_" << std::endl;
#endif
  return 480;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_ror_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_ror_mem_mode_3_" << std::endl;
#endif
  return 481;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_ror_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_ror_mem_mode_4_" << std::endl;
#endif
  return 482;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_ror_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_ror_mem_mode_5_" << std::endl;
#endif
  return 483;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_ror_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_ror_mem_mode_6_" << std::endl;
#endif
  return 484;
}

unsigned DEC_fetch_oper_store_reg_zero_shift_zero_ror_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_zero_shift_zero_ror_mem_mode_7_" << std::endl;
#endif
  return 485;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsl_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsl_mem_mode_0_" << std::endl;
#endif
  return 486;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsl_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsl_mem_mode_1_" << std::endl;
#endif
  return 487;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsl_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsl_mem_mode_2_" << std::endl;
#endif
  return 488;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsl_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsl_mem_mode_3_" << std::endl;
#endif
  return 489;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsl_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsl_mem_mode_4_" << std::endl;
#endif
  return 490;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsl_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsl_mem_mode_5_" << std::endl;
#endif
  return 491;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsl_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsl_mem_mode_6_" << std::endl;
#endif
  return 492;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsl_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsl_mem_mode_7_" << std::endl;
#endif
  return 493;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsr_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsr_mem_mode_0_" << std::endl;
#endif
  return 494;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsr_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsr_mem_mode_1_" << std::endl;
#endif
  return 495;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsr_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsr_mem_mode_2_" << std::endl;
#endif
  return 496;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsr_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsr_mem_mode_3_" << std::endl;
#endif
  return 497;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsr_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsr_mem_mode_4_" << std::endl;
#endif
  return 498;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsr_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsr_mem_mode_5_" << std::endl;
#endif
  return 499;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsr_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsr_mem_mode_6_" << std::endl;
#endif
  return 500;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_lsr_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_lsr_mem_mode_7_" << std::endl;
#endif
  return 501;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_asr_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_asr_mem_mode_0_" << std::endl;
#endif
  return 502;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_asr_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_asr_mem_mode_1_" << std::endl;
#endif
  return 503;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_asr_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_asr_mem_mode_2_" << std::endl;
#endif
  return 504;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_asr_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_asr_mem_mode_3_" << std::endl;
#endif
  return 505;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_asr_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_asr_mem_mode_4_" << std::endl;
#endif
  return 506;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_asr_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_asr_mem_mode_5_" << std::endl;
#endif
  return 507;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_asr_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_asr_mem_mode_6_" << std::endl;
#endif
  return 508;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_asr_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_asr_mem_mode_7_" << std::endl;
#endif
  return 509;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_ror_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_ror_mem_mode_0_" << std::endl;
#endif
  return 510;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_ror_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_ror_mem_mode_1_" << std::endl;
#endif
  return 511;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_ror_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_ror_mem_mode_2_" << std::endl;
#endif
  return 512;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_ror_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_ror_mem_mode_3_" << std::endl;
#endif
  return 513;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_ror_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_ror_mem_mode_4_" << std::endl;
#endif
  return 514;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_ror_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_ror_mem_mode_5_" << std::endl;
#endif
  return 515;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_ror_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_ror_mem_mode_6_" << std::endl;
#endif
  return 516;
}

unsigned DEC_fetch_oper_store_reg_imm_shift_i_ror_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_reg_imm_shift_i_ror_mem_mode_7_" << std::endl;
#endif
  return 517;
}

unsigned DEC_fetch_oper_store_ext_imm_mem_mode_2_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_ext_imm_mem_mode_2_" << std::endl;
#endif
  return 518;
}

unsigned DEC_fetch_oper_store_ext_imm_mem_mode_3_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_ext_imm_mem_mode_3_" << std::endl;
#endif
  return 519;
}

unsigned DEC_fetch_oper_store_ext_imm_mem_mode_6_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_ext_imm_mem_mode_6_" << std::endl;
#endif
  return 520;
}

unsigned DEC_fetch_oper_store_ext_imm_mem_mode_7_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_ext_imm_mem_mode_7_" << std::endl;
#endif
  return 521;
}

unsigned DEC_fetch_oper_store_ext_reg_mem_mode_0_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_ext_reg_mem_mode_0_" << std::endl;
#endif
  return 522;
}

unsigned DEC_fetch_oper_store_ext_reg_mem_mode_1_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_ext_reg_mem_mode_1_" << std::endl;
#endif
  return 523;
}

unsigned DEC_fetch_oper_store_ext_reg_mem_mode_4_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_ext_reg_mem_mode_4_" << std::endl;
#endif
  return 524;
}

unsigned DEC_fetch_oper_store_ext_reg_mem_mode_5_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "store_ext_reg_mem_mode_5_" << std::endl;
#endif
  return 525;
}

unsigned DEC_fetch_oper_swap_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "swap_" << std::endl;
#endif
  return 526;
}

unsigned DEC_fetch_oper_ldm_update_rn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "ldm_update_rn_" << std::endl;
#endif
  return 527;
}

unsigned DEC_fetch_oper_ldm_noupdate_rn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "ldm_noupdate_rn_" << std::endl;
#endif
  return 528;
}

unsigned DEC_fetch_oper_stm_update_rn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "stm_update_rn_" << std::endl;
#endif
  return 529;
}

unsigned DEC_fetch_oper_stm_noupdate_rn_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "stm_noupdate_rn_" << std::endl;
#endif
  return 530;
}

unsigned DEC_fetch_oper_mult_mla_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_mla_" << std::endl;
#endif
  return 531;
}

unsigned DEC_fetch_oper_mult_mlas_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_mlas_" << std::endl;
#endif
  return 532;
}

unsigned DEC_fetch_oper_mult_mul_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_mul_" << std::endl;
#endif
  return 533;
}

unsigned DEC_fetch_oper_mult_muls_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_muls_" << std::endl;
#endif
  return 534;
}

unsigned DEC_fetch_oper_mult_long_smull_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_long_smull_" << std::endl;
#endif
  return 535;
}

unsigned DEC_fetch_oper_mult_long_smulls_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_long_smulls_" << std::endl;
#endif
  return 536;
}

unsigned DEC_fetch_oper_mult_long_smlal_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_long_smlal_" << std::endl;
#endif
  return 537;
}

unsigned DEC_fetch_oper_mult_long_smlals_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_long_smlals_" << std::endl;
#endif
  return 538;
}

unsigned DEC_fetch_oper_mult_long_umull_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_long_umull_" << std::endl;
#endif
  return 539;
}

unsigned DEC_fetch_oper_mult_long_umulls_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_long_umulls_" << std::endl;
#endif
  return 540;
}

unsigned DEC_fetch_oper_mult_long_umlal_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_long_umlal_" << std::endl;
#endif
  return 541;
}

unsigned DEC_fetch_oper_mult_long_umlals_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "mult_long_umlals_" << std::endl;
#endif
  return 542;
}

unsigned DEC_fetch_oper_syscall_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "syscall_" << std::endl;
#endif
  return 543;
}

unsigned DEC_fetch_oper_fpe_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "fpe_" << std::endl;
#endif
  return 544;
}

unsigned DEC_fetch_oper_coproc_ld_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "coproc_ld_" << std::endl;
#endif
  return 545;
}

unsigned DEC_fetch_oper_coproc_st_(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "coproc_st_" << std::endl;
#endif
  return 546;
}

unsigned DEC_fetch_oper_unknown(_INST_T inst) {
#ifdef _DUMP_DECODE
std::cerr << "unknown" << std::endl;
#endif
  return 547;
}

