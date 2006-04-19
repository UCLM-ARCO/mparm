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


#include "define.hpp"


_STR_T reg_names[16]  = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "sl", "fp", "ip", "sp", "lr", "pc", };

_STR_T cond_names[16]  = {"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc", "hi", "ls", "ge", "lt", "gt", "le", "", "nv", };

_STR_T mem_sizes[8]  = {"", "b", "h", "", "", "", "", "", };

_STR_T mem_signs[2]  = {"", "s", };

_STR_T arith_signs[2]  = {"-", "", };

_UINT_T(16) pred_table[16]  = {0xf0f0, 0xf0f, 0xcccc, 0x3333, 0xff00, 0xff, 0xaaaa, 0x5555, 0xc0c, 0xf3f3, 0xaa55, 0x55aa, 0xa05, 0xf5fa, 0xffff, 0x0, };

