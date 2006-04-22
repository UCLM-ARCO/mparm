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
#undef _MAIN_DECODE_ENTRY
#undef _FUNC_DEFAULT

typedef _UINT_T(32)::BASE_T _INST_T;
#define _MAIN_DECODE_ENTRY  unsigned DEC_fetch_oper(_INST_T inst)
extern _MAIN_DECODE_ENTRY;
#define _FUNC_DEFAULT DEC_fetch_oper_unknown
