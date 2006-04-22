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


#ifndef __DEFN_DEC_H__
#define __DEFN_DEC_H__


#include "interface.hpp"


extern _STR_T reg_names[16];

extern _STR_T cond_names[16];

extern _STR_T mem_sizes[8];

extern _STR_T mem_signs[2];

extern _STR_T arith_signs[2];

extern _UINT_T(16) pred_table[16];

extern _UINT_T(32) syn_pc;

extern _UINT_T(32) syn_iw;

_DIRTY_E_FUN_DEC(sys_call, const _UINT_T(32)&);

_DIRTY_E_FUN_DEC(fpe_emul, const _UINT_T(32)&, const _UINT_T(32)&);

_DIRTY_E_FUN_DEC(rmb, _UINT_T(4)&, const _UINT_T(32)&, const _UINT_T(4)&);

_DIRTY_E_FUN_DEC(ldstm_syntax, _STR_T&, const _UINT_T(32)&);

_DIRTY_E_FUN_DEC(fpe_syntax, _STR_T&, const _UINT_T(32)&);

_DIRTY_E_FUN_DEC(fatal_error, const _UINT_T(32)&);

#endif
