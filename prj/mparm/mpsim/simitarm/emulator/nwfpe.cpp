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

#include "misc.h"
#include "nwfpe.h"

#ifdef __COMPILE_SIMULATOR_
#include "armsim.hpp"
#else 
#include "armemul.h"
using emulator::INST_FPE;
#endif

#ifdef DEBUG
#include <cstdio>
#endif

#define fpe_stack (FPE_STACK-72)

#define FNFLD ((inst>>16) & 7)
#define FDFLD ((inst>>12) & 7)
#define FCFLD (inst & 15)

#undef DEBUG
/* if currently in fpe emulation */

void impl_fpe_w_pc(armulator *emu, arm_inst_t inst, arm_addr_t pc)
{
	int i;

	/* if already in fpe, then must be an exit */
	if (emu->in_fpe) {

#ifdef DEBUG
		fprintf(stderr, "exit from FPE\n");
#endif

		if (READ_REG(0)==0) {
			fprintf(stderr, "floating point error!\n");
			fprintf(stderr, "r[0] = %x\n", READ_REG(0));
			//fprintf(stderr, "fpsr = %x\n\n", SA_MEM_READ_WORD(FPE_FPA11+80));
			fprintf(stderr, "fpsr = %x\n\n", nwfpe_cache_read_word(emu,FPE_FPA11+80));
		}

		/* restore registers */
		for (i=0;i<16;i++)
			//WRITE_REG(i, SA_MEM_READ_WORD(fpe_stack+i*4));
			WRITE_REG(i, nwfpe_cache_read_word(emu,fpe_stack+i*4));
		/* restore CPSR */
#ifndef __COMPILE_SIMULATOR_
		//WRITE_CPSR(SA_MEM_READ_WORD(fpe_stack+64));
		WRITE_CPSR(nwfpe_cache_read_word(emu,fpe_stack+64))
#else
		//WRITE_CPSR((SA_MEM_READ_WORD(fpe_stack+64))>>28);
		WRITE_CPSR((nwfpe_cache_read_word(emu,fpe_stack+64))>>28);
#endif

		emu->in_fpe = false;
	}
	else {
		emu->in_fpe = true;

#ifdef DEBUG
		{
		char buf[256];
		fprintf(stderr, "emulate an FPE %08x:%08x\n", pc-8, inst);
		disasm_fpe(inst, pc-8, buf);
		fprintf(stderr, "0x%08x : 0x%08x    %s", pc-8, inst, buf);
		}
#endif

		/* save registers */
		for (i=0;i<15;i++) {
			//SA_MEM_WRITE_WORD(fpe_stack+i*4, READ_REG(i));
			nwfpe_cache_write_word(emu, fpe_stack+i*4, READ_REG(i));
		}

		/* program counter set to the next instruction */
		//SA_MEM_WRITE_WORD(fpe_stack+i*4, pc-4);
		nwfpe_cache_write_word(emu, fpe_stack+i*4, pc-4);
#ifndef __COMPILE_SIMULATOR_
		//SA_MEM_WRITE_WORD(fpe_stack+64, READ_CPSR);
		nwfpe_cache_write_word(emu, fpe_stack+64, READ_CPSR);
#else
		//SA_MEM_WRITE_WORD(fpe_stack+64, (READ_CPSR)<<28);
		nwfpe_cache_write_word(emu, fpe_stack+64, (READ_CPSR)<<28);
#endif

		/* r10 points to the fp working area */
		WRITE_REG(10, FPE_FPA11);

		/* SP pointes to the userregister area */
		WRITE_REG(SPIND, fpe_stack);

		/* PC goes to the fpe entry point */
		WRITE_REG(15, FPE_ENTRY);

#ifndef __COMPILE_SIMULATOR_
		EMULATOR_STUB(fpe, inst);
#endif

	}
}

void impl_fpe(armulator *emu, arm_inst_t inst)
{
	impl_fpe_w_pc(emu, inst, PC);
}

static char *arm_binfpnames[] = 
{"adf", "muf", "suf", "rsf", "dvf", "rdf", "pow", "rpw",
 "rmf", "fml", "fdv", "frd", "pol"};

static char *arm_unfpnames[] =
{"mvd", "mnf", "abs", "rnd", "sqt", "log", "lgn", "exp",
 "sin", "cos", "tan", "asn", "acs", "atn", "urd", "nrm"};

static char *arm_fp_const[] =
{"0.0", "1.0", "2.0", "3.0", "4.0", "5.0", "0.5", "10.0"};

/*precision string */
static char fpe_prec(arm_inst_t inst) 
{
	switch (inst & 0x00080080)
	{
		case 0: return 's';
		case 0x80: return 'd';
		default: return 'e';
	} 
}

static char fpe_prec2(arm_inst_t inst)
{
	switch (inst & 0x00408000)
	{
		case 0: return 's';
		case 0x8000: return 'd';
		case 0x00400000: return 'e';
		default: return 'p';
	}
}

static char fpe_count(arm_inst_t inst)
{
	switch (inst & 0x00408000)
	{
		case 0: return '4';
		case 0x8000: return '1';
		case 0x00400000: return '2';
		default: return '3';
	}
}

static char fpe_rmode(arm_inst_t inst)
{
	switch (inst & 0x60)
	{
		case 0: return ' ';
		case 0x20: return 'p';
		case 0x40: return 'm';
		default: return 'z';                             
	}
}

static UInt32 disasm_fpe_operand(int reg, char *buf)
{
	if (reg > 7)
		return sprintf(buf, "#%s", arm_fp_const[reg & 7]);
	else
		return sprintf(buf, "f%d", reg);
}

static char *disasm_fpe_address(int inst, char *buf)
{
	buf += sprintf(buf, "[%s", arm_regnames [(inst >> 16) & 0xf]);
	if ((inst & 0x01000000) != 0)
	{
		int offset = inst & 0xff;
		if (offset)
			buf += sprintf(buf, ", %s#%d]%s",
				((inst& 0x00800000) == 0 ? "-" : ""), offset * 4,
   				((inst& 0x00200000) != 0 ? "!" : ""));
		else
			buf += sprintf(buf, "]");
	}
	else 
	{
		int offset = inst & 0xff;
		if (offset)
			buf += sprintf(buf, "], %s#%d",
				((inst & 0x00800000) == 0 ? "-" : ""), offset * 4);
		else
			buf += sprintf(buf, "]");
	}      
	return buf;
}

char *disasm_fpe(arm_inst_t inst, arm_addr_t addr, char *buf)
{
	/* binary operations */
	if ((inst & 0x00008f10) == 0x00100 &&
		OPCD>=0xE0 && OPCD<=0xEC) {

		buf += sprintf(buf, "%s%s%c%c ", arm_binfpnames[OPCD&0xF],
			arm_conditional[COND], fpe_prec(inst), fpe_rmode(inst));

		buf += disasm_fpe_operand(FDFLD, buf);
		buf += sprintf(buf, ", ");
		buf += disasm_fpe_operand(FNFLD, buf);
		buf += sprintf(buf, ", ");
		buf += disasm_fpe_operand(FCFLD, buf);
	}
	else if ((inst & 0x00008f10) == 0x08100 &&
		OPCD>=0xE0 && OPCD<=0xEF) {

		buf += sprintf(buf, "%s%s%c%c ", arm_unfpnames[OPCD&0xF],
			arm_conditional[COND], fpe_prec(inst), fpe_rmode(inst));

		buf += disasm_fpe_operand(FDFLD, buf);
		buf += sprintf(buf, ", ");
		buf += disasm_fpe_operand(FCFLD, buf);
	}
	else if ((inst & 0x0ff00f1f) == 0x0e000110) { /*flt*/
		buf += sprintf(buf, "flt%s%c%c ",
			arm_conditional[COND], fpe_prec(inst), fpe_rmode(inst));
		buf += disasm_fpe_operand(FNFLD, buf);
		buf += sprintf(buf, ", %s", arm_regnames[RDFLD]);
	}
	else if ((inst & 0x0fff0f98) == 0x0e100110) { /*fix*/
		buf += sprintf(buf, "fix%s%c ",
			arm_conditional[COND], fpe_rmode(inst));
		buf += sprintf(buf, "%s, ", arm_regnames[RDFLD]);
		buf += disasm_fpe_operand(inst&0x7, buf);
	}
	else if ((inst & 0x000f0fff) == 0x00000110 && /*wfs, rfs, wfc, rfc */
		OPCD>=0xE2 && OPCD<=0xE5) {

		static char *names[] = {"wfs", "rfs", "wfc", "rfc"};

		buf += sprintf(buf, "%s%s ", names[OPCD-0xe2], arm_conditional[COND]);
		buf += sprintf(buf, "%s", arm_regnames[RDFLD]);
	}
	else if ((inst & 0x0008fff0) == 0x0000f110 && /*cmf, cnf, cmfe, cnfe */
		(OPCD==0xE9 || OPCD == 0xEB || OPCD == 0xED || OPCD == 0xEF)) {

		static char *names[] = {"cmf", "cnf", "cmfe", "cnfe"};

		buf += sprintf(buf, "%s%s ", names[(OPCD-0xe9)>>1],
			arm_conditional[COND]);
		buf += disasm_fpe_operand(FNFLD, buf);
		buf += sprintf(buf, ", ");
		buf += disasm_fpe_operand(FCFLD, buf);
	}
	else if ((inst & 0x0e100f00) == 0x0c000100) {
		buf += sprintf(buf, "stf%s%c ", arm_conditional[COND], fpe_prec2(inst));
		buf += disasm_fpe_operand(FDFLD, buf);
		buf += sprintf(buf, ", ");
		buf = disasm_fpe_address(inst, buf);
	}
	else if ((inst & 0x0e100f00) == 0x0c100100) {
		buf += sprintf(buf, "ldf%s%c ", arm_conditional[COND], fpe_prec2(inst));
		buf += disasm_fpe_operand(FDFLD, buf);
		buf += sprintf(buf, ", ");
		buf = disasm_fpe_address(inst, buf);
	}
	else if ((inst & 0x0e100f00) == 0x0c000200) {
		buf += sprintf(buf, "sfm%s ", arm_conditional[COND]);
		buf += disasm_fpe_operand(FDFLD, buf);
		buf += sprintf(buf, ", %c, ", fpe_count(inst));
		buf = disasm_fpe_address(inst, buf);
	}
	else if ((inst & 0x0e100f00) == 0x0c100200) {
		buf += sprintf(buf, "lfm%s ", arm_conditional[COND]);
		buf += disasm_fpe_operand(FDFLD, buf);
		buf += sprintf(buf, ", %c, ", fpe_count(inst));
		buf = disasm_fpe_address(inst, buf);
	}
	else {
		buf += sprintf(buf, "????\n");
		return buf;
	}

	return buf + sprintf(buf, ";\n");
}

uint32_t nwfpe_cache_read_word(armulator *emu, target_addr_t virtual_addr)
{
	target_addr_t physical_addr;
	simulator::tlb_pte * pte;
	bool cacheable;
	bool bufferable;
	pte = emu->dtlb->read(virtual_addr);
	cacheable = pte->cacheable;
	bufferable = pte->bufferable;			
	physical_addr = emu->dtlb->get_physical_addr(pte->physical_tag,virtual_addr);
	return emu->dcache->read(physical_addr,4,cacheable,bufferable);
}	

void nwfpe_cache_write_word(armulator *emu, target_addr_t virtual_addr, uint32_t data)
{
	target_addr_t physical_addr;
	simulator::tlb_pte * pte;
	bool cacheable;
	bool bufferable;
	pte = emu->dtlb->read(virtual_addr);
	cacheable = pte->cacheable;
	bufferable = pte->bufferable;
	physical_addr = emu->dtlb->get_physical_addr(pte->physical_tag,virtual_addr);
	emu->dcache->write(physical_addr,4,true,data,cacheable,bufferable);
}	
	

