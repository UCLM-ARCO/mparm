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

#ifndef _NWFPE_H
#define _NWFPE_H

#define FPE_ENTRY 0xd0000000
#define FPE_FPA11 0xd1000000
#define FPE_STACK 0xd2000000

#define FPE_FILE "nwfpe.bin"

#include "misc.h"

#ifdef __COMPILE_SIMULATOR_
namespace simulator {
  class arm_simulator;
}
typedef simulator::arm_simulator armulator;
#else 
namespace emulator {
  class arm_emulator;
}
typedef emulator::arm_emulator armulator;
#endif

void impl_fpe(armulator *emu, arm_inst_t inst);
void impl_fpe_w_pc(armulator *emu, arm_inst_t inst, arm_addr_t pc);
char *disasm_fpe(arm_inst_t inst, arm_addr_t addr, char *buf);
uint32_t nwfpe_cache_read_word(armulator *emu, target_addr_t virtual_addr);
void nwfpe_cache_write_word(armulator *emu, target_addr_t virtual_addr, uint32_t data);

#define incr_fpe INST_FPE
#endif
