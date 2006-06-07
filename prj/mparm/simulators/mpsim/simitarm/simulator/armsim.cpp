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

#include "armsim.hpp"
#include "more_managers.hpp"

#include "machines.hpp"
#include <vector>
#include <iomanip>
#include <iostream>

#include <cstdio>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include "config.h"
#include "syscall.h"

#include "emu_device.hpp"
#include "external_dev.h"

using namespace simulator;
using emulator::memory;
using emulator::device_master;
using std::vector;
using std::list;

arm_simulator::arm_simulator(Wrapper *SAWrapper, bool verbose,UInt32 startaddr, UInt32 ICacheBlocks, UInt32 ICacheAssoc, 
				UInt32 ITLBBlocks, UInt32 ITLBAssoc, UInt32 DCacheBlocks, UInt32 DCacheAssoc,
				UInt32 DTLBBlocks, UInt32 DTLBAssoc, UInt32 WBEntries) :
	running(false), debuging(false), verbose(verbose), in_fpe(false), mem()
{
	/* create the hardware modules */
	mem = new memory(SAWrapper, startaddr);
	icache = new ICache("icache", *mem, ICacheBlocks, ICacheAssoc);
	itlb = new ITLB("itlb", *mem, ITLBBlocks, ITLBAssoc);
	dcache = new DCache("dcache", *mem, DCacheBlocks, DCacheAssoc, WBEntries);
	dtlb = new DTLB("dtlb", *mem, DTLBBlocks, DTLBAssoc);

	/* Create the managers */
#define MANG_DEF(cname, iname) iname = new cname(#iname);
#include "mang_list.hpp"

	/* conenct the managers. */
	mIF->connect(mRF, mReset, icache, itlb, mem);
	mMemCtrl->connect(dcache, dtlb, mMemRead, mMemWrite, mem, mMemAddr, mRF);
	mNewPC->connect(mRF);
	mCoProc->connect(mRF, mem);

	dev_master = new device_master();
	mCoProc->register_dev_master(dev_master);
	init_devices(dev_master);

	/* storage for operation/machine */
	machine_pool = new obj_pool<_opt_machine_, _MACH_ID_T, arm_simulator,
								arm_simulator::mach_allocator>(_mach_total);
	
	/* reset every thing */
	reset();

}

arm_simulator::~arm_simulator()
{
	delete machine_pool;

#define MANG_DEF(cname, iname) delete iname;
#include "mang_list.hpp"

	close_devices(dev_master);
	delete dev_master;

	delete icache;
	delete itlb;
	delete dcache;
	delete dtlb;
	delete mem;
}

void arm_simulator::reset()
{
	mem->reset();
	icache->reset();
	dcache->reset();
	itlb->reset();
	dtlb->reset();

#define MANG_DEF(cname, iname) iname->reset();
#include "mang_list.hpp"

	/* clear the value of cpsr */
	mCPSR->set_value(0);

	/* clear the value of the registers */
	for (int i=0; i<16; i++) {
		mRF->set_value(i, 0);
	}

	reset_stats();
	//brk_point = 0;
	//mmap_brk_point = MMAP_BASE;

	in_fpe = false;
}

void arm_simulator::reset_stats()
{
	cycle_count = 0;
	alloc_count = 0;
	retire_count = 0;
}

void arm_simulator::dump_stats(FILE *fp)
{
	icache->PrintStats(fp);
	itlb->PrintStats(fp);
	dcache->PrintStats(fp);
	dtlb->PrintStats(fp);

	fprintf(fp, "Total allocated OSMs : %lld\n", alloc_count);
	fprintf(fp, "Total retired OSMs   : %lld\n", retire_count);

	fprintf(fp, "Total cycles         : ");
	dump_int64(cycle_count, fp);
	fprintf(fp, "\nEquivalent time on 206.4MHz host: %.4f sec.\n",
		(double)cycle_count/206438400.0);

	fprintf(stderr, "Total memory pages allocated : %d\n",
		 mem->get_page_count());
}


void arm_simulator::clock_tick()
{

#undef DEBUG 
#undef DEBUG_CYCLE
#ifdef DEBUG_CYCLE
		fprintf(stderr, "--clock %d--\n", cycle_count);
#endif

		/** Activate all OSMs in order. */
		for (work_it=work_list.begin(); work_it!=work_list.end();) {

			ins_it = work_it;
			ins_it++;
#if defined DEBUG_CYCLE || defined DEBUG
			int oldstate = (*work_it)->__get_state();
#endif
			bool ret = (*work_it)->__activate();
#ifdef DEBUG_CYCLE
			std::cerr << (*work_it)->__get_dynid() << " " << oldstate << "->";
			std::cerr << (*work_it)->__get_state() << " " << ret << std::endl;
#endif

			/* Test if the OSM is back to the initial state. */
			if (ret) { /* no */
				work_it++;
			}
			else { /* yes */

				/* send retired ones to the sink. */
				sink_machine(*work_it);
				work_it = work_list.erase(work_it);
			}
		}

		cycle_count++;

		mMemCtrl->update_on_clock();
		mNewPC->update_on_clock();
		mIF->update_on_clock();
		mReset->update_on_clock();
		mEX->update_on_clock();
		mCoProc->update_on_clock();

		dcache->updateOnClockTick();
}

void arm_simulator::initialize()
{
	_opt_machine_ *src = source_machine();
	work_list.push_back(src);
	running = true;
}

void arm_simulator::run(UInt64 maxCycle)
{
	initialize();
	while(running && cycle_count<=maxCycle) {
		clock_tick();
	}
}

void arm_simulator::activate_machine(_MACH_ID_T id)
{
	_opt_machine_ *mach = allocate_machine(id);
	mach->__init();

	work_list.insert(ins_it, mach);
}

_opt_machine_ *arm_simulator::allocate_machine(_MACH_ID_T id)
{
	_opt_machine_ *mach = machine_pool->allocate(this,id);
	mach->__set_dynid(alloc_count++);
	return mach;
}

void arm_simulator::sink_machine(_opt_machine_ *mach)
{
	retire_count++;
	mach->__retire();
	machine_pool->reclaim(mach);
}

_opt_machine_ *arm_simulator::source_machine()
{
	_opt_machine_ *src = allocate_machine(_mach_fetch_normal);
	src->__init();

	return src;
}

_opt_machine_ *arm_simulator::mach_allocator(arm_simulator *s, _MACH_ID_T id)
{
  switch (id) {
#define _MACH_DEF(_a, _b) \
	case _mach_ ## _b: return new _MACH_CLASS(_b)(s);
#include "mach_list.hpp"
	default:
      assert(0);
  }
}

