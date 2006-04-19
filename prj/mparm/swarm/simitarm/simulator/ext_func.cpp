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

#include <iostream>
#include "interface.hpp"
#include "syscall.h"
#include "nwfpe.h"

using std::ostream;

#ifdef _GEN_DISASSEMBLER
#include <sstream>
using std::stringstream;
#endif

using simulator::arm_simulator;

#define UFLD ((inst>>23)&1)
#define PFLD ((inst>>24)&1)
#define WFLD ((inst>>21)&1)
#define LFLD ((inst>>20)&1)

_UINT_T(32) syn_pc, syn_iw;

extern _STR_T cond_names[16];
extern _STR_T reg_names[16];

#ifndef _GEN_DISASSEMBLER
/* external functions defined */
void sys_call(arm_simulator *sm, const _UINT_T(32)& iw)
{
	do_syscall(sm, iw.val());
}
#endif

/* get the right most bit from bit ind, for ldm/stm simulation use */
void rmb(arm_simulator *sm, _UINT_T(4)& val, const _UINT_T(32)& ival, const _UINT_T(4)& ind)
{
	unsigned v = ival.val();
	for (int i=ind.val(); i<16; i++) {
		if (v&(1<<i)) {
			val=i;
			return;
		}
	}
	/* if ival==0, then the result is not set. */ 
}

static void disasm_ldstm(arm_inst_t inst, ostream& strm)
{
	UInt32 i, first = 1;
	
	strm << (LFLD?"ldm":"stm") << cond_names[COND]
		<< (UFLD?"i":"d") << (PFLD?"b":"a") << " "
		<< reg_names[RNFLD] << (WFLD?"!":"") << ", {";

	for (i = 0; i<16; i++) {
		if ((1<<i) & inst) {
			strm << (first?"":", ") << reg_names[i];
			first = 0;
		}
	}
	strm << "}";

	if (BITn(inst,22)) strm << "^";
}

void ldstm_syntax(arm_simulator *sm, _STR_T &val, const _UINT_T(32)& iw)
{
#ifdef _GEN_DISASSEMBLER
	stringstream sstr;
	disasm_ldstm(iw.val(), sstr);
	val = sstr.str();
#endif
}

void fatal_error(arm_simulator *sim, const _UINT_T(32)& iw)
{
	fprintf(stderr, "Unknown opcode 0x%08x reached buffer stage.\n", iw.val() );
}

#ifndef _GEN_DISASSEMBLER
void fpe_emul(arm_simulator *sm, const _UINT_T(32)& iw, const _UINT_T(32)& pc) 
{
	impl_fpe_w_pc(sm, iw.val(), pc.val());
}
#endif

void fpe_syntax(arm_simulator *sm, _STR_T &val, const _UINT_T(32)& iw)
{
#ifdef _GEN_DISASSEMBLER
	char buf[256];
	disasm_fpe(iw.val(), 0, buf);
	val = buf;
#endif
}
