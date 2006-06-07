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

#ifndef __ARMSIMUL_H__
#define __ARMSIMUL_H__

#include <list>


#include "parms.h"

#include "more_managers.hpp"
#include "object_pool.hpp"

/* forward declaration*/
class Wrapper;

/* forward declaration*/
namespace emulator {
	class memory;
	class device_master;
}

namespace simulator {

typedef class rcache<ICacheLineSize> ICache; 
typedef class rwcache<DCacheLineSize> DCache;
typedef class rcache_tlb<IPageSize> ITLB;
typedef class rcache_tlb<DPageSize> DTLB;

/* forward declaration */
class device_emulator;

class arm_simulator {

	enum _machine_id {
		_mach_dummy,
		#define _MACH_DEF(_a,_b) _mach_ ## _b,
		#include "mach_list.hpp"
		_mach_total,
	};

	public:

		/** Constructor.
		 *  @param verbose Verbose mode
		 */
		arm_simulator(Wrapper *SAWrapper, bool verbose, UInt32 startaddr, UInt32 ICacheBlocks, UInt32 ICacheAssoc, 
				UInt32 ITLBBlocks, UInt32 ITLBAssoc, UInt32 DCacheBlocks, UInt32 DCacheAssoc,
				UInt32 DTLBBlocks, UInt32 DTLBAssoc, UInt32 WBEntries);

		/** Destructor. */
		~arm_simulator();

		/** Initialize the simulation state.
		 */
		void initialize();

		/** simulate for n cycles.
		 *  @param n The maximum number of cycles to simulate (may exit).
		 */
		void run(UInt64 n);

		/** simulate one cycle. */
		void clock_tick();

		/** stop running. 
		 *  Called by the signal handler or syscall interpreter.
		 */
		void stop() {running = false;}

		/** See if the simulator is running.
		 */
		bool is_running() const {return running;}

		/** Stop debuging, useless here.
		 *  Kept only to satisfy syscall.cpp.
		 */
		void stop_debug() {debuging = false;}

		/** See if in debugging mode, useless here.
		 *  Kept only to satisfy syscall.cpp.
		 */
		bool is_debugging() {return debuging;}

		/** stats output. */
		void dump_stats(FILE *fp);

		/** stats resetting. */
		void reset_stats();

		/* set brk point for syscall interpretation. */
		void set_brk(arm_addr_t addr) {brk_point = addr;}
		void set_mmap_brk(arm_addr_t addr) {mmap_brk_point = addr;}

		/** load the program into memory. */
		void load_program(const char *, int argc, char *argv[], char *env[]);

		/** load fpe emulation library into memory. */
		void load_fpe(const char *);

		/** reset */
		void reset();

		/** Get the value of a register. For debugging use. */
		word_t read_gpr(int ind) const {
			return mRF->get_value(ind).val();
		}

		/** These two hooks for the syscall interpretor. */
		void write_gpr(int ind, word_t val) {
			mRF->set_value(ind, val);
		}

		void write_gpr2(int ind, word_t val) {
			mRF->set_value(ind, val);
		}

		uint64_t get_cycle_count() const {return cycle_count;}

		word_t read_cpsr() {return mCPSR->get_value().val();}
		void write_cpsr(word_t val) {mCPSR->set_value(val);}

#if 0
		word_t read_spsr() {return my_regs.spsr;}

		void write_spsr(word_t val) {my_regs.spsr=val;}
#endif


	private:
		/* status flags */
		bool running;
		bool debuging;
		uint64_t cycle_count;	/* number of simulation cycles*/

		std::list<_opt_machine_ *> work_list;
		std::list<_opt_machine_ *>::iterator work_it, ins_it;

	public:
		/* syscall data */
		bool verbose;
		bool emulate_syscall;
		target_addr_t brk_point;
		target_addr_t mmap_brk_point;

		/* emulating fpe */
		bool in_fpe;

	public:

		uint64_t alloc_count;	/* number of machine allocations. */
		uint64_t retire_count;	/* number of machine retires. */

		/* Allocate a machine of given id type. */
		_opt_machine_ *allocate_machine(_MACH_ID_T id);

		 /* This is the sink of OSMs. */
		void sink_machine(_opt_machine_ *mach);

		/* Get a new machine.
		 * This is the source of OSMs.
		 */
		_opt_machine_ *source_machine();

		/** Activate a new machine.
		 *  @param id The id of the machine.
		 */
		void activate_machine(_MACH_ID_T id);

	private:


		static _opt_machine_ *mach_allocator(arm_simulator *, _MACH_ID_T id);

		/* storage for machine objects. */
		obj_pool<_opt_machine_, _MACH_ID_T, arm_simulator,
	   		arm_simulator::mach_allocator> *machine_pool;

		/*hardware units */

	public:

		/* caches, TLBs & bus interface unit, modeling timing only */
		ICache *icache;
		DCache *dcache;
		ITLB *itlb;
		DTLB *dtlb;
		//BIU *biu;

		/* the token managers */
#define MANG_DEF(cname, iname) cname *iname;
#include "mang_list.hpp"

		emulator::device_master *dev_master;

		/* memory saving the real data value */
		emulator::memory *mem;

};

}

#endif
