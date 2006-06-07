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

#ifndef __EMU_MEMORY_H__
#define __EMU_MEMORY_H__

#include "misc.h"
#include <cstring>
#include <iostream>

#ifdef COSIM_STUB
#include <map>
#include "external_mem.h"
#endif


/* forward declaration */
class Wrapper;

namespace emulator {

const unsigned int MEMORY_PAGE_SIZE = 4096;
const unsigned int PRIMARY_MEMORY_HASH_TABLE_SIZE = 4096;
const unsigned int SECONDARY_MEMORY_HASH_TABLE_SIZE = 16;
const unsigned int WORD_NUMBER_IN_PAGE = MEMORY_PAGE_SIZE/4;	// number of word in a page

typedef struct memory_page_table_entry_t
{
	target_addr_t virtual_addr;
	struct memory_page_table_entry_t *next;
	byte_t *storage;
	target_addr_t physical_addr;
	bool cacheable;
	bool bufferable;
} memory_page_table_entry_t;

typedef struct
{
	memory_page_table_entry_t *pte[SECONDARY_MEMORY_HASH_TABLE_SIZE];
} secondary_memory_hash_table_t;

class memory
{

#ifdef COSIM_STUB
		std::map<target_addr_t, int> external_decoded_addr;
#endif

	public:

		/* constructor */
		memory(Wrapper *SAWrapper, UInt32 startaddr);

		/* destructor */
		~memory();

		/* copy constructor, not implemented */
		memory(const memory&);

		/* free all memory pages */
		void reset();

		/* read operations */
		halfword_t read_half_word(target_addr_t addr);
		byte_t read_byte(target_addr_t addr);
		word_t read_word(target_addr_t addr);
		void read_burst(uint32_t *data, target_addr_t addr, uint32_t size);

		/* write operations */
		void set_block_bypass(target_addr_t virtual_addr, byte_t value, unsigned int size);
		void write_block_bypass(target_addr_t virtual_addr, void *buf, unsigned int size);
		void write_burst(uint8_t *data, target_addr_t addr, uint32_t size, bool blocking);

		bool check_bus_busy();

		memory_page_table_entry_t *get_page(target_addr_t virtual_addr)
		{
			memory_page_table_entry_t *pte;
	
			virtual_addr = virtual_addr - (virtual_addr % MEMORY_PAGE_SIZE);
			pte = search_page(virtual_addr);
			if(!pte)
				pte = allocate_page(virtual_addr);

			return pte;
		}

		/* some usefule utilities */
		target_addr_t align_to_page_boundary(target_addr_t addr);

#ifdef TEST_MEMORY
		/* memory test , true->succeed */
		bool test();
#endif

		/* return the number of pages allocated */
		unsigned int get_page_count() {
			return page_count;
		}

#ifdef COSIM_STUB                                   
		int register_addr(target_addr_t addr);                      
#endif
			   
	private:

		static halfword_t swap_half_word(halfword_t val) {
			return (val>>8) | (val<<8);
		}

		static word_t swap_word(word_t val) {
			return (val>>24) | ((val>>8)&0xFF00) |
				   ((val&0xFF00)<<8) |  (val<<24) ;
		}

		static dword_t swap_dword(dword_t val) {
			return ((dword_t)swap_word((word_t)val)<<32) |
				    (dword_t)swap_word((word_t)(val>>32));
		}

		static UInt32 hash1(target_addr_t addr)
		{
			return (addr / MEMORY_PAGE_SIZE) % PRIMARY_MEMORY_HASH_TABLE_SIZE;
		}

		static UInt32 hash2(target_addr_t addr)
		{
			return (addr / MEMORY_PAGE_SIZE / PRIMARY_MEMORY_HASH_TABLE_SIZE) %
			   SECONDARY_MEMORY_HASH_TABLE_SIZE;
		}

		memory_page_table_entry_t *allocate_page(target_addr_t virtual_addr);

		memory_page_table_entry_t *search_page(target_addr_t virtual_addr)
		{
			UInt32 h1;
			UInt32 h2;
			secondary_memory_hash_table_t *secondary_hash_table;
	
			virtual_addr = virtual_addr - (virtual_addr % MEMORY_PAGE_SIZE);
	
			h1 = hash1(virtual_addr);
			secondary_hash_table = primary_hash_table[h1];
	
			if(secondary_hash_table)
			{
				memory_page_table_entry_t *pte;
				h2 = hash2(virtual_addr);
				pte = secondary_hash_table->pte[h2];
		
				while(pte)
				{
					if(pte->virtual_addr == virtual_addr) return pte;
					pte = pte->next;
				}
			}
			return 0;
		}

		secondary_memory_hash_table_t 
			*primary_hash_table[PRIMARY_MEMORY_HASH_TABLE_SIZE];

		unsigned int page_count;	/*stats information */

		UInt32 startaddr;
		Wrapper *mywrapper;
		uint8_t *WB_entry_temp;
};

} /* namespace */

#endif





