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

#include "emumem.h"
#ifdef COSIM_STUB
#include "external_mem.h"
#endif

#include <simitarm_wrapper.h>
#include "address.h"

#include <cstring>
#include <iostream>

using namespace emulator;

memory::memory(Wrapper *SAWrapper, UInt32 startaddr) : startaddr(startaddr)
{
	WB_entry_temp = new uint8_t[writeBufferEntrySize];
	memset(primary_hash_table, 0, sizeof(primary_hash_table));
	page_count = 0;
	mywrapper = SAWrapper;
}

memory::~memory()
{
	reset();
}

memory::memory(const memory& mem)
{
	unsigned int i, j;
	memset(primary_hash_table, 0, sizeof(primary_hash_table));
	page_count = 0;
	
	for(i = 0; i < PRIMARY_MEMORY_HASH_TABLE_SIZE; i++)
	{
		const secondary_memory_hash_table_t *secondary_hash_table =
		   mem.primary_hash_table[i];
		
		if(secondary_hash_table)
		{
			secondary_memory_hash_table_t *secondary_hash_table_cpy = 
				new secondary_memory_hash_table_t;
			memset(secondary_hash_table_cpy, 0,
				sizeof(secondary_memory_hash_table_t));
			primary_hash_table[i] = secondary_hash_table_cpy;

			for(j = 0; j < SECONDARY_MEMORY_HASH_TABLE_SIZE; j++)
			{
				const memory_page_table_entry_t *pte = 
					secondary_hash_table->pte[j];

				while (pte)
				{
					memory_page_table_entry_t *pte_cpy =
						new memory_page_table_entry_t;
					pte_cpy->virtual_addr = pte->virtual_addr;
					pte_cpy->storage = new byte_t[MEMORY_PAGE_SIZE];
					memcpy(pte_cpy->storage, pte->storage, MEMORY_PAGE_SIZE);  // se utilizzata: attenti a pte->storage!
					pte_cpy->next = secondary_hash_table_cpy->pte[j];
					secondary_hash_table_cpy->pte[j] = pte_cpy;
					page_count++;

					pte = pte->next;
				}
			}
		}
	}
}

void memory::reset()
{
	unsigned int i, j;
	
	for(i = 0; i < PRIMARY_MEMORY_HASH_TABLE_SIZE; i++)
	{
		secondary_memory_hash_table_t *secondary_hash_table =
		   primary_hash_table[i];
		
		if(secondary_hash_table)
		{
			for(j = 0; j < SECONDARY_MEMORY_HASH_TABLE_SIZE; j++)
			{
				memory_page_table_entry_t *pte = secondary_hash_table->pte[j];
				memory_page_table_entry_t *nextpte;

				while (pte)
				{
					nextpte = pte->next;
					delete pte;
					pte = nextpte;
				}
			}
			delete secondary_hash_table;
		}
	}
	memset(primary_hash_table, 0, sizeof(primary_hash_table));
	page_count = 0;
}



// ***************************** ALLOCATE PAGE ***************************************

memory_page_table_entry_t *memory::allocate_page(target_addr_t virtual_addr)
{
	UInt32 h1, h2;
	memory_page_table_entry_t *pte;
	secondary_memory_hash_table_t *secondary_hash_table;
		
	h1 = hash1(virtual_addr);
	secondary_hash_table = primary_hash_table[h1];
		
	if(!secondary_hash_table)
	{
		secondary_hash_table = new secondary_memory_hash_table_t;
		memset(secondary_hash_table, 0,
			sizeof(secondary_memory_hash_table_t));
		primary_hash_table[h1] = secondary_hash_table;
	}
		
	h2 = hash2(virtual_addr);
	pte = new memory_page_table_entry_t;
	pte->virtual_addr = virtual_addr;
	pte->physical_addr = page_count * MEMORY_PAGE_SIZE;
	pte->storage = (byte_t*) (startaddr + pte->physical_addr);			// allocating a new page on ExtRam
	if (addresser->LogicalIsCacheable(pte->physical_addr))
	{
		pte->cacheable = true;
		pte->bufferable = true;
	}
	else
	{
		pte->cacheable = false;
		pte->bufferable = false;
		printf("where are you going to?\n");
	}	
	memset(pte->storage, 0, MEMORY_PAGE_SIZE);
	pte->next = secondary_hash_table->pte[h2];
	secondary_hash_table->pte[h2] = pte;
	//std::cout << "new page allocated:	page_count = " << std::dec << page_count << "     if_bus: 0x"<< std::hex << pte->physical_addr << "     iv: 0x"<< std::hex << virtual_addr << "     if_extmem: 0x"<< std::hex << (uint)pte->storage << std::endl;
	page_count++;
	return pte;
}

#ifdef COSIM_STUB
int memory::register_addr(target_addr_t addr) {
	static int uniq_id = 0;
	return (external_decoded_addr[addr] = uniq_id++);
}
#endif

// ***************************** READ BYTE ***************************************

byte_t memory::read_byte(target_addr_t addr)
{
	target_addr_t offset = addr % MEMORY_PAGE_SIZE;
	word_t value = 0;

#ifdef COSIM_STUB   
	if (external_decoded_addr.find(addr) != external_decoded_addr.end()) {
		return ext_read_byte(external_decoded_addr[addr]);
	}                                                   
#endif
	mywrapper->gotobus(&value,addr,0,1,1);
	value = value >> ((offset & 3) * 8);					// word shift
	return (byte_t)value;
}


// ***************************** READ WORD ***************************************

word_t memory::read_word(target_addr_t addr)
{
	word_t value = 0;

#ifdef COSIM_STUB
	if (external_decoded_addr.find(addr) != external_decoded_addr.end()) {
		return ext_read_word(external_decoded_addr[addr]);
	}
#endif
	mywrapper->gotobus(&value,addr,0,4,1);
	return value;
}


// ***************************** READ HALF WORD ***************************************

halfword_t memory::read_half_word(target_addr_t addr)
{
	target_addr_t offset = addr % MEMORY_PAGE_SIZE;
	word_t value = 0;
	
	mywrapper->gotobus(&value,addr,0,2,1);
	value = value >> ( (offset & 3) * 8);					// word shift
	
#ifdef COSIM_STUB
	if (external_decoded_addr.find(addr) != external_decoded_addr.end()) {
		return ext_read_half_word(external_decoded_addr[addr]);
	}
#endif
	return (halfword_t)value;
}


// ***************************** WRITE BLOCK BYPASS***************************************

void memory::write_block_bypass(target_addr_t virtual_addr, void *buf, unsigned int size)	// writing directly on ExtRam: program loading
{
	if(size > 0)
	{
		UInt32 offset = virtual_addr % MEMORY_PAGE_SIZE;
		memory_page_table_entry_t *pte = get_page(virtual_addr);
		UInt32 sz = MEMORY_PAGE_SIZE - offset;	
		if(size > sz)
		{
			memcpy(pte->storage + offset, buf, sz);
			size -= sz;
			virtual_addr += sz;
			buf = (byte_t *) buf + sz;
			
			if(size >= MEMORY_PAGE_SIZE)
			{
				do
				{
					pte = get_page(virtual_addr);
					memcpy(pte->storage, buf, MEMORY_PAGE_SIZE);
					size -= MEMORY_PAGE_SIZE;
					virtual_addr += MEMORY_PAGE_SIZE;
					buf = (byte_t *) buf + MEMORY_PAGE_SIZE;
				} while(size >= MEMORY_PAGE_SIZE);
			}
		
			if(size > 0)
			{
				pte = get_page(virtual_addr);
				memcpy(pte->storage, buf, size);
			}
		}
		else
		{
			memcpy(pte->storage + offset, buf, size);
		}
	}
}

// ***************************** SET BLOCK BYPASS ***************************************

void memory::set_block_bypass(target_addr_t virtual_addr, byte_t value, unsigned int size)		// writing directly on ExtRam: program loading
{
	if(size > 0)
	{
		UInt32 offset = virtual_addr % MEMORY_PAGE_SIZE;
		memory_page_table_entry_t *pte = get_page(virtual_addr);
		UInt32 sz = MEMORY_PAGE_SIZE - offset;	
		if(size > sz)
		{
	
			memset(pte->storage + offset, value, sz);
			size -= sz;
			virtual_addr += sz;
			
			if(size >= MEMORY_PAGE_SIZE)
			{
				do
				{
					pte = get_page(virtual_addr);
					memset(pte->storage, value, MEMORY_PAGE_SIZE);
					size -= MEMORY_PAGE_SIZE;
					virtual_addr += MEMORY_PAGE_SIZE;
				} while(size >= MEMORY_PAGE_SIZE);
			}
		
			if(size > 0)
			{
				pte = get_page(virtual_addr);
				memset(pte->storage, value, size);
			}
		}
		else
		{
			memset(pte->storage + offset, value, size);
		}
	}
}

// ***************************** READ BURST ***************************************

void memory::read_burst(uint32_t *data, target_addr_t addr, uint32_t size)
{
	mywrapper->gotobus(data,addr,0,4*size,1);
}

// ***************************** WRITE BURST ***************************************

void memory::write_burst(uint8_t *data, target_addr_t addr, uint32_t size, bool blocking)
{
	if(!blocking)
	{
		for (unsigned int i=0;i<size;i++)  //// NOTA ?????????'					// evita overwriting nel WB
			*(WB_entry_temp+i) = *(data+i);
		mywrapper->gotobus((uint32_t *)WB_entry_temp,addr,1,size,0);
	}
	else
		mywrapper->gotobus((uint32_t *)data,addr,1,size,1);
}

bool memory::check_bus_busy()
{
	return mywrapper->bus_busy;
}

target_addr_t memory::align_to_page_boundary(target_addr_t addr)
{
	return addr - (addr % MEMORY_PAGE_SIZE);
}

#ifdef TEST_MEMORY
bool memory::test()			// not supported now!!!
{
	unsigned int i;
	bool ret = true;

	/* test half_word */
	halfword_t hw;
	for (i=0; i<1024;i+=2)
		write_half_word(i, (halfword_t)i);

	for (i=0; i<1024;i+=2)
		if ((hw=read_half_word(i))!=(halfword_t)i ||
			read_byte(i)!=(byte_t)i ||
			read_byte(i+1)!=(byte_t)(i>>8)) {
			ret = false;
			std::cerr << "halfword test failed at " << i << ", get " << hw
				  << std::endl;
		}

	/* test word */
	word_t w;
	for (i=0; i<1024;i+=4)
		write_word(i, (word_t)i);

	for (i=0; i<1024;i+=4)
		if ((w=read_word(i))!=i ||
			read_byte(i)!=(byte_t)i ||
			read_byte(i+3)!=(byte_t)(i>>24)) {
			ret = false;
			std::cerr << "word test failed at " << i << ", get " << w
				  << std::endl;
		}

	/* test dword */
	dword_t dw;
	for (i=0; i<1024;i+=8)
		write_dword(i, ((dword_t)i<<32)|i);

	for (i=0; i<1024;i+=8)
		if ((dw=read_dword(i))!=(((dword_t)i<<32)|i) ||
			read_byte(i)!=(byte_t)i ||
			read_byte(i+7)!=(byte_t)(i>>24)) {
			ret = false;
			std::cerr << "dword test failed at " << i << ", get " << dw 
				  << std::endl;
		}


	/* test block */
	char hello[] = "hello world!\n";
	char bufff[sizeof(hello)];
	

	write_block(0xffff, hello, sizeof(hello));
	read_block(bufff, 0xffff, sizeof(hello));

	for (i=0; i<sizeof(hello);i+=8)
		if (bufff[i]!=hello[i]) {
			ret = false;
			std::cerr << "block test failed at " << i << ", get " << bufff[i]
				  << std::endl;
		}

	/* test copy constructor using word */
	for (i=0; i<1024;i+=4)
		write_word(i, (word_t)i);

	memory mem(*this);
	for (i=0; i<1024;i+=4)
		if ((w=mem.read_word(i))!=i ||
			mem.read_byte(i)!=(byte_t)i ||
			mem.read_byte(i+3)!=(byte_t)(i>>24)) {
			ret = false;
			std::cerr << "copy test failed at " << i << ", get " << w
				  << std::endl;
		}

	if (mem.page_count != page_count)
		std::cerr << "page count mismatch after copying" << std::endl;

	return ret;
}
#endif

