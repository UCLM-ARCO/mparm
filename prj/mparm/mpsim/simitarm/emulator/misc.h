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

#ifndef __MISC_H__
#define __MISC_H__

#include "config.h"
#include "globals.h"           // Data types
#include "bittypes.h"
#include <cstdio>

#define COND (inst>>28)
#define OPCD ((inst>>20)&255)
#define DOPC ((inst>>21)&15)	/*DPI's opcode*/

#define RDFLD ((inst>>12)&15)
#define RNFLD ((inst>>16)&15)
#define RMFLD (inst&15)
#define RSFLD ((inst>>8)&15)

#define WRITE_REG(ind, val) (emu->write_gpr(ind,val))
#define READ_REG(ind) (emu->read_gpr(ind))

#define PC_UPDATED (emu->is_pc_updated())
#define PC_UPDATE_CLEAR (emu->clear_pc_update())

#define WRITE_REG2(ind, val) (emu->write_gpr2(ind,val))

#define READ_CPSR (emu->read_cpsr())
#define READ_SPSR (emu->read_spsr())

#define WRITE_CPSR(val) (emu->write_cpsr(val))
#define WRITE_SPSR(val) (emu->write_spsr(val))

#define CPSR READ_CPSR
#define SPSR READ_SPSR

#define FPIND 11
#define IPIND 12
#define SPIND 13
#define LRIND 14
#define PCIND 15

#define RD READ_REG(RDFLD)
#define RN READ_REG(RNFLD)
#define RM READ_REG(RMFLD)
#define RS READ_REG(RSFLD)
#define FP READ_REG(FPIND)
#define IP READ_REG(IPIND)
#define SP READ_REG(SPIND)
#define LR READ_REG(LRIND)
#define PC READ_REG(PCIND)

#define SA_N_FLAG ((CPSR>>31)&1)	//ridefinito!
#define SA_Z_FLAG ((CPSR>>30)&1)	//ridefinito!
#define SA_C_FLAG ((CPSR>>29)&1)	//ridefinito!
#define SA_V_FLAG ((CPSR>>28)&1)	//ridefinito!

#define SET_N WRITE_CPSR(CPSR|(1<<31))
#define SET_Z WRITE_CPSR(CPSR|(1<<30))
#define SET_C WRITE_CPSR(CPSR|(1<<29))
#define SET_V WRITE_CPSR(CPSR|(1<<28))

#define CLEAR_N WRITE_CPSR(CPSR&(~(1<<31)))
#define CLEAR_Z WRITE_CPSR(CPSR&(~(1<<30)))
#define CLEAR_C WRITE_CPSR(CPSR&(~(1<<29)))
#define CLEAR_V WRITE_CPSR(CPSR&(~(1<<28)))

#define ASGN_N(n) {if (n) SET_N; else CLEAR_N;}
#define ASGN_Z(n) {if (n) SET_Z; else CLEAR_Z;}
#define ASGN_C(n) {if (n) SET_C; else CLEAR_C;}
#define ASGN_V(n) {if (n) SET_V; else CLEAR_V;}

#define ASM_CONDS(n,z,c,v) (((n)<<3)|((z)<<2)|((c)<<1)|(v))
#define ASGN_CONDS(cond) WRITE_CPSR((CPSR&0xfffffff)|(cond<<28))

#define SFLD ((inst>>20)&1)

//#define SA_MEM_READ_WORD(addr) (emu->mem->read_word(addr))
//#define SA_MEM_WRITE_WORD(addr,val) (emu->mem->write_word(addr,val))

//#define SA_MEM_READ_HALF_WORD(addr) (emu->mem->read_half_word(addr))
//#define SA_MEM_WRITE_HALF_WORD(addr,val) (emu->mem->write_half_word(addr,val))

//#define SA_MEM_READ_BYTE(addr) (emu->mem->read_byte(addr))
//#define SA_MEM_WRITE_BYTE(addr,val) (emu->mem->write_byte(addr,val))


/* stack grows to smaller address, mmap area grows toward bigger address */
/* 0xd0000000 to MMAP_BASE is used by floating point emulation, see nwfpe.h */
#define STACK_BASE 0xc0000000
#define MMAP_BASE  0xd4000000
#define MAX_ENVIRON (16 * 1024)
#define STACK_SIZE (1024 * 1024)

#define BIT31(a) ((UInt32)(a)>>31)
#define BIT0(a)  ((a)&1)
#define BITn(a,b) (((UInt32)(a)>>(b))&1)

#define IN_PRVLG (((CPSR)&0x1F)!=0x10)
#define HAS_SPSR ((((CPSR)&0x1F)!=0x10) && (((CPSR)&0x1F)!=0x1F))

#ifdef COUNT_INST
#define EMULATOR_STUB(a,b) emu->increment_counter(incr_ ## a);
#else
#define EMULATOR_STUB(a,b)
#endif

extern const char *arm_conditional[];
extern const char *arm_regnames[];
extern const char *arm_shift[];

uint64_t ato_uint64(char *input);

void dump_int64(uint64_t, FILE *);
void dump_int64_smart(uint64_t, FILE *);

/*target endianness*/
#define TARGET_LITTLE_ENDIAN 1

/*host endianness*/
#ifndef WORDS_BIGENDIAN
#define WORDS_BIGENDIAN 0
#endif

#endif
