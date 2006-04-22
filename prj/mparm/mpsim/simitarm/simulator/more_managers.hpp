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

#ifndef __MORE_MANAGERS_HPP__
#define __MORE_MANAGERS_HPP__

/* include libosm stuff */
#include "include_osm.hpp"
#include "emumem.h"
#include "parms.h"
#include "sa_cache.h"
#include <iostream>

/** simple resource manager. */
typedef unit_manager<int> simple_resource;
typedef unit_value_manager<int> dummy_resource;

/* forward declaration */
namespace emulator {
	class device_master;
};

namespace simulator {

/* forward declaration*/
typedef class rcache<ICacheLineSize> ICache;
typedef class rwcache<DCacheLineSize> DCache;
typedef class rcache_tlb<IPageSize> ITLB;
typedef class rcache_tlb<DPageSize> DTLB;

/** The register file. */
class regfile_manager {

	public:

		/** index type. */
		typedef _UINT_T(4)  I;

		/** value type. */
		typedef _UINT_T(32) T;

		/** token type. */
		typedef class _token_<regfile_manager> TOKEN_T;

		/** Constructor.
		 *  @param name Name of the register file
		 */
		regfile_manager(const std::string& name) : name(name) {

			tokens = new TOKEN_T[16](this);
			values = new T[16];
		}

		/** Destructor. */
		~regfile_manager() {
			delete [] tokens;
			delete [] values;
		}

		/** See if token is available for allocation.
		 *  @param ind The index of the token.
		 *  @param obj The OSM sending the request.			
		 */
		bool token_free_for_alloc(const I& ind, _BASE_MACHINE *obj) {
			return tokens[ind.val()].is_free() 
				/*|| obj->__get_dynid() >
					tokens[ind.val()].get_tenant()->__get_dynid()*/;
		}

		/** See if token is available for inquire.
		 *  @param ind The index of the token.
		 *  @param obj The OSM sending the request.			
		 */
		bool token_free_for_inquire(const I& ind, _BASE_MACHINE *obj) {
			return tokens[ind.val()].is_free() ||
				   obj==tokens[ind.val()].get_tenant();
		}

		/** See if it is ok to release token.
		 *  @param token The token to release.
		 *  @param obj The OSM sending the request.			
		 */
		bool token_good_to_reclaim(TOKEN_T *token, _BASE_MACHINE *obj) {
			return true;
		}

		/** Allocate a token.
		 *  @param ind The index of the token.
		 *  @param obj The OSM sending the request.			
		 */
		TOKEN_T *allocate_token(const I& ind, _BASE_MACHINE *obj) {
			tokens[ind.val()].set_used(obj);
			return tokens+ind.val();
		}

		void inquire_token(const I& ind, _BASE_MACHINE *obj) {
		}

		/** Release a token.
		 *  @param token The token to release.
		 *  @param obj The OSM sending the request.			
		 */
		void reclaim_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			if (token->get_tenant()==obj)
				token->set_unused();
		}

		/** Discard a token.
		 *  @param token The token to discard.
		 *  @param obj The OSM sending the request.			
		 */
		void discard_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			if (token->get_tenant()==obj)
				token->set_unused();
		}

		/** Get the name of the manager. */
		const std::string& get_name() {
			return name;
		}


		/** Access the value of the token.
		 * This corresponds to val = *m[ind] in OSM expression.
		 * @param ind The index of the token.
		 * @param obj The OSM reading the value.
		 */
		T read_token(const I& ind, _BASE_MACHINE *obj) {
			return values[ind.val()];
		}

		/** Access the value of the token.
		 * This corresponds to val = *token in OSM expression.
		 * @param obj The OSM reading the value.
		 */
		T read_token(_BASE_MACHINE *obj, TOKEN_T *token) {
			return values[token-tokens];
		}

		/** Update the value of the token.
		 * This function is called from the OSM. This
		 * corresponds to *m[ind] = val in OSM expression.
		 * To prevent double write in the same cycle, we allocate_temp first.
		 * @param ind The index of the token.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 */
		void write_token(const I& ind, const T& val, _BASE_MACHINE *obj) {
			values[ind.val()] = val;
		}													 

		/** Update the value of the token.
		 * This function is called from the token. This corresponds
		 * to *token = val in OSM expression.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 * @param token The token to be updated.
		 */
		void write_token(const T& val, _BASE_MACHINE *obj, TOKEN_T *token) {
			values[token-tokens] = val;
		}


		/** Get the value of the program counter. */
		T get_pc() const {
			return values[15];
		}

		/** Set the value of the program counter. */
		void set_pc(const T& val) {
			values[15] = val;
		}

		/** Get the value of a register.
		 *  This is used for value access from the hardware layer.
		 *  @param ind Index of the token.
		 */
		T get_value(int ind) {
			return values[ind];
		}

		/* Set the value of a register.
		 * This is used for value access from the hardware layer. So
		 * this does not monitor the value of pc.
		 * @param ind Index of the token.
		 * @param val Value to write.
		 */
		void set_value(int ind, const T& val) {
			values[ind] = val;
		}

		/** Free those temporarily allocated. */
		void update_on_clock() {}

		/** Resetting states. */
		void reset() {
			for (int i=0; i<16; i++) {
				tokens[i].set_unused();
				values[i] = 0;
			}
		}

	private:

		std::string name;

		TOKEN_T *tokens;
		T *values;

};

class reset_manager {

	public:

		/** Token value type. */
		typedef _TUPLE_T(_UINT_T(4),_UINT_T(1)) TVAL;

		/** Constructor.
		 *  @param name Name of the manager.
		 */
		reset_manager(const std::string& name) :
			name(name), trigger(false) {}

		/** Destructor. */
		~reset_manager() {}

		/** See if the token is good for allocation, dummy, always true.
		 *  @param obj The requesting OSM.
		 */
		bool token_free_for_inquire(_BASE_MACHINE *obj) {
			return trigger;
		}

		/** See if the token is good for allocation, dummy, always true.
		 *  Called by writers.
		 *  @param obj The requesting OSM.
		 */
		bool token_free_for_alloc(_BASE_MACHINE *obj) {
			return true;
		}

		void inquire_token(_BASE_MACHINE *obj) {
		}

		/** Get the name of the manager. */
		const std::string& get_name() {
			return name;
		}

		/** Update the value of the token.
		 * This function is called from the OSM. This
		 * corresponds to *m[] = val in OSM expression.
		 * To prevent double write in the same cycle, we allocate_temp first.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 */
		void write_token(const TVAL& val, _BASE_MACHINE *obj) {
			/* second is the predicate bit */
			if (val.first.val()==15 && val.second.val()>0) {
				trigger = true;
			}
		}

		/** query from the ifetcher */
		bool branch_taken() {return trigger;}

		/** Regular update on clock edge. */
		void update_on_clock() {
			reset();
		}

		/** Reset states. */
		void reset() {
			trigger = false;
		}

	private:

		std::string name;
		bool trigger;

};

/** fetching manager. */
class fetch_manager {

	public:

		/** The token type. */
		typedef class _token_<fetch_manager> TOKEN_T;

		/** The constructor.
		 *  @param name Name of the manager.
		 */
		fetch_manager(const std::string& name) :
			name(name), token(new TOKEN_T(this)),
	   		fetch_status(0), trigger(false) {}

		/** Destructor. */
		~fetch_manager() {
			delete token;
		}


		/** See if the token is available for allocation.
		 *  @param obj The requesting OSM.
		 */
		bool token_free_for_alloc(_BASE_MACHINE *obj) {
			return token->is_free();
		}

		/** See if the token is available for inquire.
		 *  @param obj The requesting OSM.
		 */
		bool token_free_for_inquire(_BASE_MACHINE *obj) {
			return token->is_free();
		}

		/** See if the token is available for release.
		 *  @param obj The requesting OSM.
		 */
		bool token_good_to_reclaim(TOKEN_T *token, _BASE_MACHINE *obj) {
			return fetch_status==0;
		}

		/** Allocate a token.
		 *  @param obj The OSM sending the request.			
		 */
		TOKEN_T *allocate_token(_BASE_MACHINE *obj) {
			token->set_used(obj);
			fetch_status = 1;
			trigger = true;
			return token;
		}

		void inquire_token(_BASE_MACHINE *obj) {
		}

		/** Release a token.
		 *  @param token The token to release.
		 *  @param obj The OSM sending the request.			
		 */
		void reclaim_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			token->set_unused();
		}

		/** Discard a token.
		 *  @param token The token to discard.
		 *  @param obj The OSM sending the request.			
		 */
		void discard_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			token->set_unused();
		}

		/** Get the name of the manager. */
		const std::string& get_name() {return name;}

		/** Access the value of the token.
		 * This corresponds to val = *token in OSM expression.
		 * @param obj The OSM reading the value.
		 */
		_UINT_T(32) read_token(_BASE_MACHINE *obj, TOKEN_T *token) {

			uint32_t physical_addr = itlb->get_physical_addr(pte->physical_tag,current_pc);
			unsigned iw = icache->read(physical_addr,pte->cacheable);
			return iw;
		}

		/** Connect the fetcher with other modules.
		 *  @param mrf  The register file manager, for program counter.
		 *  @param csh  The instruction cache object.
		 *  @param tlb  The instruction tlb object.
		 *  @param imm  The memory to get instrution from.
		 */
		void connect(regfile_manager *mrf, reset_manager *mrs, 
			ICache *csh, ITLB *tlb, emulator::memory *imm) {
		   	mRF = mrf;
			mRS = mrs;
			icache = csh;
			itlb = tlb;
			mem = imm;
		}

		/** Regular state update on clock edge. */
		void update_on_clock();

		/** Reset the states/ */
		void reset() {
			trigger = false;
			token->set_unused();
			fetch_status = 0;
		}

	private:

		std::string name;
		TOKEN_T *token;

		unsigned current_pc;	/* the pc currently fetching */
		int fetch_status;		/* state of fetching. */
		int trigger;			/* start a new request. */
		bool cacheable;
		tlb_pte *pte;

		/* connected components. */
		regfile_manager *mRF;
		reset_manager *mRS;
		ICache *icache;
		ITLB   *itlb;
		emulator::memory *mem;
};

typedef class unit_value_manager<_UINT_T(32)> mem_addr_port;
typedef class unit_value_manager<_UINT_T(32)> mem_read_port;
typedef class unit_value_manager<_UINT_T(32)> mem_write_port;
typedef class unit_value_manager<_UINT_T(4)> cpsr_manager;

/** The memory control port.
 *  This port does nothing but to synchronize data memory access.
 *  An OSM should inquire this port to see if the memory request finishes.
 */
class mem_ctrl_port {

	public:

		/** Constructor.
		 *  @param name Name of the manager.
		 */
		mem_ctrl_port(const std::string& name) :
			name(name), count(0), status(0), mergeable(false), trigger(false) {}

		/** Destructor. */
		~mem_ctrl_port() {}

		/** See if token is availble for inquire.
		 *  Equivalent to if the memory access has finished.
		 *  @param obj The object performing the memory transaction.
		 */
		bool token_free_for_inquire(_BASE_MACHINE *obj) {
			return status==0;
		}

		/** See if the token is available for allocate, dummy always true. */
		bool token_free_for_alloc(_BASE_MACHINE *obj) {
			return true;
		}

		void inquire_token(_BASE_MACHINE *obj) {
		}

		/** Update the value of the token.
		 * This function is called from the OSM. This
		 * corresponds to *m[] = val in OSM expression.
		 * To prevent double write in the same cycle, we allocate_temp first.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 */
		void write_token(const _UINT_T(32)& val, _BASE_MACHINE *obj) {
			isRead = 1 - (val.val() & 1);
			size = (val.val()>>1)&7;
			count = val.val()>>4;
			trigger = true;
		}

		/** Get the name of the manager. */
		const std::string& get_name() {
			return name;
		}

		/** Connect the module with other modules.
		 *  @param csh  The data cache object.
		 *  @param tlb  The data tlb object.
		 *  @param rpt  The memory read  port.
		 *  @param wpt  The memory write port.
		 *  @param dmm  The data memory containing real data.
		 *  @param map  The memory address port.
		 *  @param reg  The register file.
		 */
		void connect(DCache *csh, DTLB *tlb,
			mem_read_port *rpt, mem_write_port *wpt, emulator::memory *dmm,
			mem_addr_port *map, regfile_manager *reg) {
			dcache = csh;
			dtlb = tlb;
			mMemRead = rpt;
			mMemWrite = wpt;
			mem = dmm;
			mMemAddr = map;
			mRF = reg;
		}

		/** Regular update on clock edge. */
		void update_on_clock();

		/** reset states. */
		void reset() {
			status = 0;
		}

	private:

		std::string name;

		unsigned count;
		unsigned status;
		unsigned virtual_addr;
		unsigned size;
		bool mergeable;
		bool isRead;
		bool trigger;
		bool cacheable;
		bool bufferable;
		tlb_pte *pte;

		DCache *dcache;
		DTLB   *dtlb;
		mem_read_port *mMemRead;
		mem_write_port *mMemWrite;
		mem_addr_port *mMemAddr;
		regfile_manager *mRF;
		emulator::memory *mem;

};


/** The new program counter manager.
 *  Branch instruction write result to this.
 */
class new_pc {

	public:

		/** Token value type. */
		typedef _TUPLE_T(_UINT_T(32),_UINT_T(1)) TVAL;

		/** Constructor.
		 *  @param name Name of the manager.
		 */
		new_pc(const std::string& name) : name(name) {}

		/** Destructor. */
		~new_pc() {}

		/** See if the token is good for allocation, dummy, always true.
		 *  @param obj The requesting OSM.
		 */
		bool token_free_for_alloc(_BASE_MACHINE *obj) {
			return true;
		}

		/** Get the name of the manager. */
		const std::string& get_name() {
			return name;
		}

		/** Update the value of the token.
		 * This function is called from the OSM. This
		 * corresponds to *m[] = val in OSM expression.
		 * To prevent double write in the same cycle, we allocate_temp first.
		 * @param val The updated value.
		 * @param obj The OSM doing the update.
		 */
		void write_token(const TVAL& val, _BASE_MACHINE *obj) {

			/* second is the predicate bit */
			if (val.second.val()>0) {
				npc = val.first.val();
				trigger = true;
			}
		}

		/** Connect the fetcher with other modules.
		 *  @param mrf  The register file manager, for program counter.
		 */
		void connect(regfile_manager *mrf) {
		   	mRF = mrf;
		}

		/** Regular update on clock edge. */
		void update_on_clock() {
			if (trigger) {
				mRF->set_pc(npc);
				trigger = false;
			}
		}

		/** Reset states. */
		void reset() {
			trigger = false;
		}

	private:

		std::string name;
		bool trigger;
		_WORD_T npc;

		regfile_manager *mRF;
};


class alu_manager {

	public:

		/** Token value type. */
		typedef _UINT_T(32) TVAL;

		/** token type. */
		typedef class _token_<alu_manager> TOKEN_T;

		/** Constructor.
		 *  @param name Name of the manager.
		 */
		alu_manager(const std::string& name) :
			name(name), token(new TOKEN_T(this)) {}

		/** Destructor. */
		~alu_manager() {delete token;}

		/** See if the token is good for allocation.
		 *  @param obj The requesting OSM.
		 */
		bool token_free_for_alloc(_BASE_MACHINE *obj) {
			return token->is_free();
		}

		/** See if the token is good for inquire.
		 *  @param obj The requesting OSM.
		 */
		bool token_free_for_inquire(_BASE_MACHINE *obj) {
			return token->is_free();
		}

		/** See if it is ok to release token.
		 *  @param token The token to release.
		 *  @param obj The OSM sending the request.			
		 */
		bool token_good_to_reclaim(TOKEN_T *token, _BASE_MACHINE *obj) {
			return latency==0;
		}

		/** Allocate a token.
		 *  @param obj The OSM sending the request.			
		 */
		TOKEN_T *allocate_token(_BASE_MACHINE *obj) {
			token->set_used(obj);
			return token;
		}

		void inquire_token(_BASE_MACHINE *obj) {
		}

		/** Release a token.
		 *  @param token The token to release.
		 *  @param obj The OSM sending the request.			
		 */
		void reclaim_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			token->set_unused();
		}

		/** Release a token.
		 *  @param token The token to release.
		 *  @param obj The OSM sending the request.			
		 */
		void discard_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			token->set_unused();
			latency = 0;
		}

		/** Get the name of the manager. */
		/** Get the name of the manager. */
		const std::string& get_name() {
			return name;
		}

		/** Update the value of the token.
		 *  This function is called from the token. This corresponds
		 *  to *token = val in OSM expression.
		 *  @param val The updated value.
		 *  @param obj The OSM doing the update.
		 *  @param token The token to be updated.
		 */
		void write_token(const TVAL& val, _BASE_MACHINE *obj, TOKEN_T *token) {

			/* this is written by mult operation, whose latency depends
			 * on operand value. */
			_S_WORD_T rs_val = val.val();

			if ((rs_val>>11)==-1 || (rs_val>>11)==0) latency = 1;
			else if ((rs_val>>23)==-1 || (rs_val>>23)==0) latency = 2;
			else latency = 3;
		}

		/** Regular update on clock edge. */
		void update_on_clock() {
			if (latency>0) latency--;
		}

		/** Reset. */
		void reset() {
			token->set_unused();
			latency = 0;
		}

	private:

		std::string name;
		TOKEN_T *token;
		int latency;
};

class coproc_manager {

	public:

		/** index type. */
		typedef _TUPLE_T(_UINT_T(1),_UINT_T(1)) I;

		/** token type. */
		typedef class _token_<coproc_manager> TOKEN_T;

		/** Constructor.
		 *  @param name Name of the register file
		 */
		coproc_manager(const std::string& name) : name(name), state(0) {
			token = new TOKEN_T(this);
		}

		/** Destructor. */
		~coproc_manager() {
			delete token;
		}

		/** See if token is available for inquire.
		 *  @param ind The index of the token.
		 *  @param obj The OSM sending the request.			
		 */
		bool token_free_for_alloc(const I& ind, _BASE_MACHINE *obj) {
			return token->is_free();
		}

		/** See if it is ok to release token.
		 *  @param token The token to release.
		 *  @param obj The OSM sending the request.			
		 */
		bool token_good_to_reclaim(TOKEN_T *token, _BASE_MACHINE *obj) {
			return state==0;
		}

		/** Allocate a token.
		 *  @param ind The index of the token.
		 *  @param obj The OSM sending the request.			
		 */
		TOKEN_T *allocate_token(const I& ind, _BASE_MACHINE *obj) {
			state = ind.first.val()?2:1;
			has_addr = ind.second.val();
			return token;
		}

		/** Release a token.
		 *  @param token The token to release.
		 *  @param obj The OSM sending the request.			
		 */
		void reclaim_token(TOKEN_T *token, _BASE_MACHINE *obj) {
			token->set_unused();
		}

		/** Get the name of the manager. */
		const std::string& get_name() {
			return name;
		}

		/** Connect the fetcher with other modules.
		 *  @param mrf  The register file manager, for program counter.
		 *  @param imm  The memory to get instrution from.
		 */
		void connect(regfile_manager *mrf, emulator::memory *imm) {
		   	mRF = mrf;
			mem = imm;
		}

		/** Register the device master . */
		void register_dev_master(emulator::device_master *dev) {
			dev_emul = dev;
		}

		/** Free those temporarily allocated. */
		void update_on_clock() {
			if (state==0) return;
			else if (state==1) do_read();
			else do_write();
		}

		/** Resetting states. */
		void reset() {
			state = 0;
			token->set_unused();
		}

	private:

		std::string name;
		TOKEN_T *token;

		regfile_manager *mRF;
		emulator::memory *mem;

		bool has_addr;  // has address or not
		int state; // 0: nothing, 1 to read, 2 to write

		emulator::device_master *dev_emul;

		void do_read();
		void do_write();
};

}


#endif
