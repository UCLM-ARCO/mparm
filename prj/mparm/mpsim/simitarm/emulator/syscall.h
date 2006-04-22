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
/***************************************************************************
                          syscall.h  -  description
                             -------------------
    begin                : Wed Sep 26 2001
    copyright            : (C) 2001 CEA and Université Paris XI Orsay
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __SYSCALL_H__
#define __SYSCALL_H__


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

extern void sc_impl(armulator *emu, arm_inst_t inst);
extern char *sc_disasm(arm_inst_t inst, target_addr_t pc, char *buf);
extern void do_syscall(armulator *emu, arm_inst_t inst);
void syscall_cache_read(armulator *emu, void *buf, target_addr_t virtual_addr, unsigned int size);
void syscall_cache_write(armulator *emu, target_addr_t virtual_addr, void *buf, unsigned int size);

#define impl_sc sc_impl
#define disasm_sc sc_disasm
#define incr_sc INST_SYSCALL

#endif
