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

#ifndef __SA_CACHE_H__
#define __SA_CACHE_H__


#include <string>
#include "emumem.h"
#include "more_managers.hpp"

//classes cache, rcache, rwcache always use physical address!!!

namespace simulator {

struct cache_block {
	uint32_t tag;
	bool valid;
	bool dirty[2];
	uint32_t cache_block_data[DCacheLineSize/4];
};

struct tlb_block {
	uint32_t virtual_tag;
	bool valid;
	bool cacheable;
	bool bufferable;
	uint32_t physical_tag;
};

struct tlb_pte {
	bool cacheable;
	bool bufferable;
	uint32_t physical_tag;
};

struct WB_entry {
	uint32_t tag;
	bool valid[writeBufferEntrySize];
	uint8_t WB_entry_data[writeBufferEntrySize];
};

template <uint32_t bsize>
class cache {

	protected:

		uint32_t cache_index(uint32_t addr) {
			return (addr/bsize)%(n_block/n_assoc);
		}

		uint32_t cache_tag(uint32_t addr) {
			return (addr/(bsize*n_block/n_assoc));
		}
		
		uint32_t get_addr(uint32_t tag, uint32_t ind, bool half_line) {
			return (tag*(n_block/n_assoc)*2 + ind*2 + half_line)*(bsize/2);
		}
		
		/* ARM uses the round robin policy */
		uint32_t choose_round_robin(uint32_t index) {
			return round_robin_index[index];
		}

		void update_round_robin(uint32_t index) {
			round_robin_index[index] = (round_robin_index[index]+1)%n_assoc;
		}

		/* check if the address is in cache */
		struct cache_block *lookup(uint32_t addr) {
			uint32_t ind = cache_index(addr), tag = cache_tag(addr);
			for (uint32_t i=0; i<n_assoc; i++)
			{
				if (data[ind][i].tag == tag && data[ind][i].valid) 
					return &data[ind][i];
			}
			return NULL;
		}

	public:
		cache(const std::string& name, uint32_t n_block, uint32_t n_assoc) : name(name), n_block(n_block), n_assoc(n_assoc)
		{	
			round_robin_index = new uint16_t[n_block/n_assoc];
			data = new struct cache_block * [n_block/n_assoc];
			for (uint32_t j = 0; j < n_block/n_assoc; j ++)
				data[j] = new struct cache_block[n_assoc];
		}
		~cache() {}

	protected:
		const std::string name;
		uint32_t n_block;
		uint32_t n_assoc;
		
		struct cache_block **data;
		uint16_t *round_robin_index;

};

template <uint32_t bsize>
class rcache : public cache<bsize> {

	public:

		/*constructor: set the name */
		rcache(const std::string& name, emulator::memory& mem, uint32_t n_block, uint32_t n_assoc) :
			cache<bsize>(name,n_block,n_assoc), mem(mem), n_block(n_block), n_assoc(n_assoc),
	   		lastAddr(-1),
		   	nReads(0), nReadMisses(0), lastBlock(NULL) 
			{
			/*	round_robin_index = new uint16_t[n_block/n_assoc];
				data = new struct cache_block * [n_block/n_assoc];
				for (uint32_t j = 0; j < n_block/n_assoc; j ++)
				data[j] = new struct cache_block[n_assoc];*/
			}

		~rcache() {}

		void reset() {	
				for (uint32_t j = 0; j < n_block/n_assoc; j ++)
					memset(data[j], 0, n_assoc * sizeof(struct cache_block));
							
				memset(round_robin_index, 0, n_block/n_assoc * sizeof(uint16_t));
				lastAddr = -1;
				nReads = 0;
				nReadMisses = 0;
				lastBlock = NULL;
			}

		void PrintStats(FILE *fp) {

			fprintf(fp, "Total %s reads:       ", name.c_str());
			dump_int64(nReads, fp);
			fprintf(fp, "\nTotal %s read misses: ", name.c_str());
			dump_int64(nReadMisses, fp);
			fprintf(fp, "\n%s hit ratio:         %.3f%%\n", name.c_str(),
				100.0*(nReads-nReadMisses)/(nReads));
		}
		
		uint32_t read(uint32_t addr, bool cacheable);
		struct cache_block * allocate_block(uint32_t addr); 

	protected:
		emulator::memory& mem;
		uint32_t n_block;
		uint32_t n_assoc;		
		uint32_t lastAddr;
		uint64_t nReads, nReadMisses;
		struct cache_block *lastBlock;
};

template <uint32_t bsize>
class rwcache : public cache<bsize> {

	public:

		/*constructor: set the name */
		rwcache(const std::string& name, emulator::memory& mem, uint32_t n_block, uint32_t n_assoc, uint32_t nWriteBufferEntries) :
			cache<bsize>(name,n_block,n_assoc), 
			mem(mem), n_block(n_block), n_assoc(n_assoc), nWriteBufferEntries(nWriteBufferEntries), lastMergeable(false),
			nReads(0), nReadMisses(0),
			nWrites(0), nWriteMisses(0), last_index(0), old_index(0), WB_empty(true)	
			{
				write_buffer = new struct WB_entry[nWriteBufferEntries];
			}

		~rwcache() {}

		void reset() {
				for (uint32_t j = 0; j < n_block/n_assoc; j ++)
					memset(data[j], 0, n_assoc * sizeof(struct cache_block));
				
				memset(round_robin_index, 0, n_block/n_assoc * sizeof(uint16_t));
				memset(write_buffer, 0, nWriteBufferEntries  * sizeof(WB_entry));
				lastMergeable = false;
				nReads = 0;
				nReadMisses = 0;
				nWrites = 0;
				nWriteMisses = 0;
				last_index = 0;
				old_index = 0;
				WB_empty = true;
							
		}


		void updateOnClockTick() {
				if ((last_index != old_index) && (!(mem.check_bus_busy())))
				{	
					free_WB_entry(false);
					//fprintf(stdout,"update eseguito\n");
				}
		}

		void PrintStats(FILE *fp) {

			fprintf(fp, "Total %s writes:       ", name.c_str());
			dump_int64(nWrites, fp);
			fprintf(fp, "\nTotal %s write misses: ", name.c_str());
			dump_int64(nWriteMisses, fp);
			fprintf(fp, "\nTotal %s reads:        ", name.c_str());
			dump_int64(nReads, fp);
			fprintf(fp, "\nTotal %s read misses:  ", name.c_str());
			dump_int64(nReadMisses, fp);
			fprintf(fp, "\n%s hit ratio:         %.3f%%\n", name.c_str(),
				100.0*(nReads+nWrites-nReadMisses-nWriteMisses)/
				(nReads+nWrites));
		}
		
		uint32_t read(uint32_t addr, uint32_t size, bool cacheable, bool bufferable);
		void write(uint32_t addr, uint32_t size, bool mergeable, uint32_t dato, bool cacheable, bool bufferable);
		struct cache_block * allocate_block(uint32_t addr);
		void flush(); 
		void write_WB(uint32_t addr, uint8_t *dato, unsigned int size, bool mergeable);
		void free_WB_entry(bool blocking);
		bool lookup_WB(uint32_t addr);
		void flush_WB();
		
	protected:
		emulator::memory& mem;
		uint32_t n_block;
		uint32_t n_assoc;
		uint32_t nWriteBufferEntries;	
		bool lastMergeable;
		uint64_t nReads, nReadMisses;
		uint64_t nWrites, nWriteMisses;
		uint32_t last_index, old_index;
		struct WB_entry *write_buffer;
		bool WB_empty;
};

template <uint32_t bsize>
class rcache_tlb {

	public:

		/*constructor: set the name */
		rcache_tlb(const std::string& name, emulator::memory& mem, uint32_t n_block, uint32_t n_assoc) :
			name(name), mem(mem), n_block(n_block), n_assoc(n_assoc), lastAddr(-1),
		   	nReads(0), nReadMisses(0), lastBlock(NULL) 
			{
			current_pte = new tlb_pte;
			round_robin_index = new uint16_t[n_block/n_assoc];
			data = new struct tlb_block *[n_block/n_assoc];
			for (uint32_t j = 0; j < n_block/n_assoc; j ++)
				data[j] = new struct tlb_block[n_assoc];
			}

		~rcache_tlb() {}

		uint32_t cache_index(uint32_t addr) {
			return (addr/bsize)%(n_block/n_assoc);
		}

		uint32_t cache_tag(uint32_t addr) {
			return (addr/(bsize*n_block/n_assoc));
		}

		/* ARM uses the round robin policy */
		uint32_t choose_round_robin(uint32_t index) {
			return round_robin_index[index];
		}

		void update_round_robin(uint32_t index) {
			round_robin_index[index] = (round_robin_index[index]+1)%n_assoc;
		}

		/* check if the address is in cache */
		struct tlb_block *lookup(uint32_t addr) {
			uint32_t ind = cache_index(addr), tag = cache_tag(addr);
			for (uint32_t i=0; i<n_assoc; i++)
			{
				if (data[ind][i].virtual_tag == tag && data[ind][i].valid) 
					return &data[ind][i];
			}
			return NULL;
		}

		void reset() {
			memset(round_robin_index, 0, n_block/n_assoc * sizeof(uint16_t));
			for (uint32_t j = 0; j < n_block/n_assoc; j ++)
				memset(data[j],0,n_assoc * sizeof(struct tlb_block));
			lastAddr = -1;
			nReads = 0;
			nReadMisses = 0;
			lastBlock = NULL;
		}

		void PrintStats(FILE *fp) {

			fprintf(fp, "Total %s reads:       ", name.c_str());
			dump_int64(nReads, fp);
			fprintf(fp, "\nTotal %s read misses: ", name.c_str());
			dump_int64(nReadMisses, fp);
			fprintf(fp, "\n%s hit ratio:         %.3f%%\n", name.c_str(),
				100.0*(nReads-nReadMisses)/(nReads));
		}

		struct tlb_pte *read(uint32_t virtual_addr);
		struct tlb_block * allocate_block(uint32_t virtual_addr);
		uint32_t get_physical_addr(uint32_t physical_tag, uint32_t virtual_addr) {
			return physical_tag*bsize+virtual_addr%bsize;
		}

	protected:
		const std::string name;
		emulator::memory& mem;
		uint32_t n_block;
		uint32_t n_assoc;		
		uint32_t lastAddr;
		uint64_t nReads, nReadMisses;
		struct tlb_block **data;
		uint16_t *round_robin_index;
		struct tlb_block *lastBlock;
		struct tlb_pte *current_pte;
};


}

#endif
