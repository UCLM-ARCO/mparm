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


_FUN_DEC(F_eval_pred, _UINT_T(1)& A_result, const _UINT_T(4)& A_cond, const _UINT_T(4)& A_flags)
{

  A_result = _U2U(16,1,((pred_table[_VAL(A_cond)]>>_VAL(A_flags))));
}



_FUN_DEC(F_rotate_right, _UINT_T(32)& A_result, const _UINT_T(32)& A_imm, const _UINT_T(32)& A_rot)
{

  _UINT_T(32) V_shift;

  V_shift = (A_rot<<0x1);
  A_result = (((A_imm>>_VAL(V_shift)))|((A_imm<<_VAL(((_UINT_CTR(32, 0x20)-V_shift))))));
}



_FUN_DEC(F_comp_flag, _UINT_T(4)& A_result_flag, const _UINT_T(1)& A_neg, const _UINT_T(1)& A_zero, const _UINT_T(1)& A_carry, const _UINT_T(1)& A_overflow)
{

  A_result_flag = ((_U2U(3,4,((_U2U(2,3,((_U2U(1,2,A_neg)<<1) | _U2U(1,2,A_zero)))<<1) | _U2U(1,3,A_carry)))<<1) | _U2U(1,4,A_overflow));
}



_FUN_DEC(F_add_w_flags, _UINT_T(32)& A_result, _UINT_T(1)& A_carry, _UINT_T(1)& A_overflow, const _INT_T(32)& A_src1, const _INT_T(32)& A_src2)
{

  _UINT_T(64) V_xsrc1;
  _UINT_T(64) V_xsrc2;
  _UINT_T(64) V_xresult;

  V_xsrc1 = _I2U(32,64,A_src1);
  V_xsrc2 = _I2U(32,64,A_src2);
  V_xresult = (V_xsrc1+V_xsrc2);
  A_carry = ((_UINT_XTR1(A_src1, 0x1f)^_UINT_XTR1(A_src2, 0x1f))^_UINT_XTR1(V_xresult, 0x20));
  A_overflow = (_UINT_XTR1(V_xresult, 0x20)^_UINT_XTR1(V_xresult, 0x1f));
  A_result = _U2U(64,32,V_xresult);
}



_FUN_DEC(F_adc_w_flags, _UINT_T(32)& A_result, _UINT_T(1)& A_carry, _UINT_T(1)& A_overflow, const _INT_T(32)& A_src1, const _INT_T(32)& A_src2, const _UINT_T(1)& A_icarry)
{

  _UINT_T(64) V_xsrc1;
  _UINT_T(64) V_xsrc2;
  _UINT_T(64) V_xresult;

  V_xsrc1 = _I2U(32,64,A_src1);
  V_xsrc2 = _I2U(32,64,A_src2);
  V_xresult = ((V_xsrc1+V_xsrc2)+_U2U(1,64,A_icarry));
  A_carry = ((_UINT_XTR1(A_src1, 0x1f)^_UINT_XTR1(A_src2, 0x1f))^_UINT_XTR1(V_xresult, 0x20));
  A_overflow = (_UINT_XTR1(V_xresult, 0x20)^_UINT_XTR1(V_xresult, 0x1f));
  A_result = _U2U(64,32,V_xresult);
}



_FUN_DEC(F_sub_w_flags, _UINT_T(32)& A_result, _UINT_T(1)& A_carry, _UINT_T(1)& A_overflow, const _INT_T(32)& A_src1, const _INT_T(32)& A_src2)
{

  _UINT_T(64) V_xsrc1;
  _UINT_T(64) V_xsrc2;
  _UINT_T(64) V_xresult;

  V_xsrc1 = _I2U(32,64,A_src1);
  V_xsrc2 = _I2U(32,64,A_src2);
  V_xresult = (V_xsrc1-V_xsrc2);
  A_carry = ((_UINT_XTR1(A_src1, 0x1f)^_UINT_XTR1(A_src2, 0x1f))^_UINT_XTR1(V_xresult, 0x20));
  A_overflow = (_UINT_XTR1(V_xresult, 0x20)^_UINT_XTR1(V_xresult, 0x1f));
  A_result = _U2U(64,32,V_xresult);
}



_FUN_DEC(F_sbc_w_flags, _UINT_T(32)& A_result, _UINT_T(1)& A_carry, _UINT_T(1)& A_overflow, const _INT_T(32)& A_src1, const _INT_T(32)& A_src2, const _UINT_T(1)& A_icarry)
{

  _UINT_T(64) V_xsrc1;
  _UINT_T(64) V_xsrc2;
  _UINT_T(64) V_xresult;

  V_xsrc1 = _I2U(32,64,A_src1);
  V_xsrc2 = _I2U(32,64,A_src2);
  V_xresult = (((V_xsrc1-V_xsrc2)-_UINT_CTR(64, 0x1))+_U2U(1,64,A_icarry));
  A_carry = ((_UINT_XTR1(A_src1, 0x1f)^_UINT_XTR1(A_src2, 0x1f))^_UINT_XTR1(V_xresult, 0x20));
  A_overflow = (_UINT_XTR1(V_xresult, 0x20)^_UINT_XTR1(V_xresult, 0x1f));
  A_result = _U2U(64,32,V_xresult);
}



_FUN_DEC(F_popcount16, _UINT_T(32)& A_result, const _UINT_T(16)& A_src)
{

  _UINT_T(32) V_v1;
  _UINT_T(32) V_v2;
  _UINT_T(32) V_v3;
  _UINT_T(32) V_v4;

  V_v1 = _U2U(16,32,(A_src&_UINT_CTR(16, 0xffff)));
  V_v2 = (((V_v1&_UINT_CTR(32, 0x5555)))+((((V_v1>>0x1))&_UINT_CTR(32, 0x5555))));
  V_v3 = (((V_v2&_UINT_CTR(32, 0x3333)))+((((V_v2>>0x2))&_UINT_CTR(32, 0x3333))));
  V_v4 = (((V_v3&_UINT_CTR(32, 0xf0f)))+((((V_v3>>0x4))&_UINT_CTR(32, 0xf0f))));
  A_result = (((V_v4&_UINT_CTR(32, 0xff)))+((((V_v4>>0x8))&_UINT_CTR(32, 0xff))));
}

