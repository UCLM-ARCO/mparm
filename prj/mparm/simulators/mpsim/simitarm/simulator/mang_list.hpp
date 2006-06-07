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
MANG_DEF(fetch_manager, mIF)
MANG_DEF(simple_resource, mID)
MANG_DEF(simple_resource, mBF)
MANG_DEF(simple_resource, mMult)
MANG_DEF(dummy_resource, mWB)
MANG_DEF(alu_manager, mEX)
MANG_DEF(cpsr_manager, mCPSR)
MANG_DEF(regfile_manager, mRF)
MANG_DEF(mem_addr_port, mMemAddr)
MANG_DEF(mem_read_port, mMemRead)
MANG_DEF(mem_write_port, mMemWrite)
MANG_DEF(mem_ctrl_port, mMemCtrl)
MANG_DEF(new_pc, mNewPC)
MANG_DEF(reset_manager, mReset)
MANG_DEF(coproc_manager, mCoProc)
#undef MANG_DEF
