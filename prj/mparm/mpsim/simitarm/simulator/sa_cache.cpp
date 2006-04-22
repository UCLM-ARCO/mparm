#include "armsim.hpp"
#include "include_osm.hpp"
#include "emumem.h"
#include "parms.h"
#include "sa_cache.h"



//#define DEBUG

using namespace simulator;

//------------------------------------------ R CACHE ---------------------------------------------------------
	
	template < uint32_t bsize >

		uint32_t rcache< bsize >::read(uint32_t addr, bool cacheable) {
			uint32_t offset_word = (addr % bsize) / 4;
			if (addr/bsize == lastAddr)
			{
				nReads++;
				return (lastBlock->cache_block_data[offset_word]);
			}	
			struct cache_block *block = lookup(addr);
			if (block)
			{
				nReads++;
				lastAddr = addr/bsize;
				lastBlock = block;
				return (block->cache_block_data[offset_word]);
			}
			else
			{
				nReadMisses++;
				nReads++;	
				if (cacheable)
				{
				block = allocate_block(addr);
				lastAddr = addr/bsize;
				lastBlock = block;
				return (block->cache_block_data[offset_word]);
				}
				else
					return mem.read_word(addr);
			}
		}
			
		template < uint32_t bsize >

		struct cache_block *rcache< bsize >::allocate_block(uint32_t addr) {

			uint32_t ind = cache_index(addr), tag = cache_tag(addr);
			cache_block *theblock = &data[ind][choose_round_robin(ind)];
			mem.read_burst(&(theblock->cache_block_data[0]),addr - addr%bsize,bsize/4);
		#ifdef DEBUG
			fprintf(stderr,"Line allocated: address 0x%08x  words:0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",addr - addr%bsize,theblock->cache_block_data[0],theblock->cache_block_data[1],theblock->cache_block_data[2],theblock->cache_block_data[3],theblock->cache_block_data[4],theblock->cache_block_data[5],theblock->cache_block_data[6],theblock->cache_block_data[7]);
		#endif	
			theblock->tag = tag;
			theblock->valid = true;
			theblock->dirty[0] = false;
			theblock->dirty[1] = false;
			update_round_robin(ind);
			return theblock;
		}
						
		template  rcache< 32 >;
				

//------------------------------------------ RW CACHE ---------------------------------------------------------


	template < uint32_t bsize >

		uint32_t rwcache< bsize >::read(uint32_t addr, uint32_t size, bool cacheable, bool bufferable) {
			uint32_t offset_word = (addr % bsize) / 4;

#ifdef DEBUG
		fprintf(stderr,"offset_word = %d \n",offset_word);
#endif
			uint32_t offset_byte = addr % 4;
#ifdef DEBUG
		fprintf(stderr,"offset_byte = %d \n",offset_byte);
#endif

			struct cache_block *block = lookup(addr);
			//		controllo di dcache hit
			if (block)
			{
				nReads++;	
				if (size==4)
				 	{	
						return (block->cache_block_data[offset_word]);
					}
					else if (size==1) 	
						{
						return (uint8_t)((block->cache_block_data[offset_word]) >> (offset_byte * 8));
						}
						else 	
						  {										
						  return (uint16_t)((block->cache_block_data[offset_word]) >> (offset_byte * 8));
						  }
			}
			else
			{
				if (lookup_WB(addr))
				{
					//fprintf(stderr,"cache conflict - WB\n");
					flush_WB();
				}				
				nReadMisses++;
				nReads++;	
				if (cacheable && bufferable)
				{
				block = allocate_block(addr);
				if (size==4) 	{	
						return (block->cache_block_data[offset_word]);
						}
					else if (size==1) 	
						{	
						return (uint8_t)((block->cache_block_data[offset_word]) >> (offset_byte * 8));
						}
						else 	{	
						  return (uint16_t)((block->cache_block_data[offset_word]) >> (offset_byte * 8));
							}
				}
				else
				{
					if (size==4)
						return mem.read_word(addr);
					else 	if (size==1)
							return mem.read_byte(addr);
						else
							return mem.read_half_word(addr);
				}
			}
		}

		template < uint32_t bsize >

		void rwcache< bsize >::write(uint32_t addr, uint32_t size, bool mergeable, uint32_t dato, bool cacheable, bool bufferable) {
			
			uint32_t offset_word = (addr % bsize) / 4;
#ifdef DEBUG
		fprintf(stderr,"offset_word = %d \n",offset_word);
#endif
			uint32_t offset_byte = addr % 4;
#ifdef DEBUG
		fprintf(stderr,"offset_byte = %d \n",offset_byte);
#endif

			struct cache_block *block = lookup(addr);
			/* cache hit, good */
			if (block) 
			{
				nWrites++;
				block->dirty[(addr/(bsize/2))&1] = true;
				if (size==4) 	
					{
						block->cache_block_data[offset_word] = dato;
					}
					else if (size==1)
					{
					 	*((uint8_t *)&(block->cache_block_data[offset_word]) + offset_byte) = (uint8_t) dato;
					}
						else 	
						{
						*((uint16_t *)&(block->cache_block_data[offset_word]) + offset_byte) = (uint16_t) dato;
						}
			}
			else
			{	
			#ifdef DEBUG		
				fprintf(stderr,"write MISS at 0x%08x data 0x%08x\n",addr,dato);
			#endif
				nWriteMisses++;
				nWrites++;
				if (cacheable && bufferable)
					write_WB(addr,(uint8_t *)&dato,size,mergeable);
				else
					mem.write_burst((uint8_t *)&dato,addr,size,true);
			}
		}
		
		template < uint32_t bsize >

		struct cache_block *rwcache< bsize >::allocate_block(uint32_t addr) {

			uint32_t ind = cache_index(addr), tag = cache_tag(addr);
			cache_block *theblock = &data[ind][choose_round_robin(ind)];
			if (theblock->valid && theblock->dirty[0]) {
				uint32_t addr_old = get_addr(theblock->tag,ind,0);
				write_WB(addr_old,(uint8_t *)&(theblock->cache_block_data[0]),writeBufferEntrySize,false);
			}
			if (theblock->valid && theblock->dirty[1]) {
			    uint32_t addr_old = get_addr(theblock->tag,ind,1);
			    write_WB(addr_old,(uint8_t *)&(theblock->cache_block_data[writeBufferEntrySize/4]),writeBufferEntrySize,false);
			}
			mem.read_burst(&(theblock->cache_block_data[0]),addr - addr%bsize,bsize/4);
		#ifdef DEBUG
			fprintf(stderr,"Line allocated: address 0x%08x words:0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",addr - addr%bsize,theblock->cache_block_data[0],theblock->cache_block_data[1],theblock->cache_block_data[2],theblock->cache_block_data[3],theblock->cache_block_data[4],theblock->cache_block_data[5],theblock->cache_block_data[6],theblock->cache_block_data[7]);
		#endif	
			theblock->tag = tag;
			theblock->valid = true;
			theblock->dirty[0] = false;
			theblock->dirty[1] = false;
			update_round_robin(ind);
			return theblock;
		}
		
		template < uint32_t bsize >
		
		void rwcache< bsize >::write_WB(uint32_t addr, uint8_t *dato, unsigned int size, bool mergeable)	
		{			
			uint32_t tag = addr/writeBufferEntrySize;
			uint32_t offset = addr % writeBufferEntrySize;
			if (mergeable && lastMergeable) {
				if (tag==write_buffer[last_index].tag) {
					lastMergeable = mergeable;
					for(unsigned int i=0;i<size;i++)
					{
					   write_buffer[last_index].WB_entry_data[offset+i] = *(dato+i);
					   write_buffer[last_index].valid[offset+i] = true;
					   #ifdef DEBUG
					     fprintf(stderr,"write_WB  addr = 0x%x   data = 0x%x  last_index = %d\n",tag*writeBufferEntrySize+offset+i,*(dato+i),last_index);
					   #endif
					}
					return;
				}
			}
			if (((last_index + 1) % nWriteBufferEntries) == old_index)
			{
				free_WB_entry(true);
				//fprintf(stderr,"WB full   old_index=%d\n",old_index);
			}
			if (!WB_empty)
				last_index = (last_index + 1) % nWriteBufferEntries;
			for(unsigned int i=0;i<size;i++)
			{
				write_buffer[last_index].WB_entry_data[offset+i] = *(dato+i);
				write_buffer[last_index].valid[offset+i] = true;
				#ifdef DEBUG
					fprintf(stderr,"write_WB    addr = 0x%x      data = 0x%x    last_index = %d\n",tag*writeBufferEntrySize+offset+i,*(dato+i),last_index);
				#endif
			}
			write_buffer[last_index].tag = tag;
			lastMergeable = mergeable;
			WB_empty = false;
		}

		template < uint32_t bsize >
		
		void rwcache< bsize >::free_WB_entry(bool blocking)	
		{

			uint32_t count = 0;
			uint32_t addr = write_buffer[old_index].tag * writeBufferEntrySize;
			#ifdef DEBUG
				fprintf(stderr,"free_WB_entry     addr iniziale = 0x%x     old_index = %d    last_index = %d\n",addr,old_index,last_index);
			#endif

			for(int i=0;i<writeBufferEntrySize;i++)
			{
				if (write_buffer[old_index].valid[i]==true)
				{	
					count++;
					write_buffer[old_index].valid[i] = false;
				}
				else 	if (count!=0)
					{
					    mem.write_burst(&(write_buffer[old_index].WB_entry_data[i-count]),addr+i-count,count,blocking);
					    count = 0;
					}
			}
			if (count!=0)
			  mem.write_burst(&(write_buffer[old_index].WB_entry_data[writeBufferEntrySize-count]),addr+writeBufferEntrySize-count,count,blocking);
			old_index = (old_index + 1) % nWriteBufferEntries;	
		}
		
		template < uint32_t bsize >
		
		bool rwcache< bsize >::lookup_WB(uint32_t addr)	
		{
			uint32_t tag_cache = addr/bsize;
			for(unsigned int i=0;i<nWriteBufferEntries;i++)
			{
				if (write_buffer[i].tag / (bsize/writeBufferEntrySize) == tag_cache)
					for (int j=0;j<writeBufferEntrySize;j++)
						if (write_buffer[i].valid[j] == true)
							return true;
			}
			return false;
		}

		template < uint32_t bsize >
		
		void rwcache< bsize >::flush_WB()	
		{
			unsigned int i = old_index;
			do	
				{
				uint32_t count = 0;
				uint32_t addr = write_buffer[i%nWriteBufferEntries].tag * writeBufferEntrySize;
				#ifdef DEBUG
					fprintf(stderr,"flush_WB  start addr = 0x%x  last_index = %d    iMOD8(old_index) = %d\n",addr,last_index,i%nWriteBufferEntries);				
				#endif
				for(int j=0;j<writeBufferEntrySize;j++)
				{
					if (write_buffer[i%nWriteBufferEntries].valid[j]==true)
					{
						count++;
						write_buffer[i%nWriteBufferEntries].valid[j] = false;
					}
					else 	if (count!=0)
						{
							mem.write_burst(&(write_buffer[i%nWriteBufferEntries].WB_entry_data[j-count]),addr+j-count,count,true);
							count = 0;
						}
				}
				if (count!=0)
					mem.write_burst(&(write_buffer[i%nWriteBufferEntries].WB_entry_data[writeBufferEntrySize-count]),addr+writeBufferEntrySize-count,count,true);
				i++;
			} while ((i-1)%nWriteBufferEntries!=last_index);
			last_index = 0;
			old_index = 0;
			lastMergeable = false;
			WB_empty = true;
		}

		template < uint32_t bsize >
		
		void rwcache< bsize >::flush() { 
				for (unsigned int i=0; i< (n_block/n_assoc) ; i++)
					for (unsigned int j=0; j < n_assoc ; j++)
					{
						cache_block *theblock = &data[i][j];
						if (theblock->valid && theblock->dirty[0]) {
							uint32_t addr_old = get_addr(theblock->tag,i,0);
							write_WB(addr_old,(uint8_t *)&(theblock->cache_block_data[0]),writeBufferEntrySize,false);
						}
						if (theblock->valid && theblock->dirty[1]) {
							uint32_t addr_old = get_addr(theblock->tag,i,1);
							write_WB(addr_old,(uint8_t *)&(theblock->cache_block_data[writeBufferEntrySize/4]),writeBufferEntrySize,false);
						}		
						theblock->valid = false;
						theblock->dirty[0] = false;
						theblock->dirty[1] = false;

					}
		}
		
		template  rwcache< 32 >;
		
//------------------------------------------ R CACHE TLB ---------------------------------------------------------
	
	
	template < uint32_t bsize >

		struct tlb_pte *rcache_tlb< bsize >::read(uint32_t virtual_addr) {
			if (virtual_addr/bsize == lastAddr)
			{
				nReads++;
				current_pte->physical_tag = lastBlock->physical_tag;
				current_pte->cacheable = lastBlock->cacheable;
				current_pte->bufferable = lastBlock->bufferable;
				return current_pte;
			}
			struct tlb_block *block = lookup(virtual_addr);	
			if (block)
			{
				nReads++;
				lastAddr = virtual_addr/bsize;
				lastBlock = block;		
				current_pte->physical_tag = lastBlock->physical_tag;
				current_pte->cacheable = lastBlock->cacheable;
				current_pte->bufferable = lastBlock->bufferable;
				return current_pte;
			}
			else
			{
				nReadMisses++;
				nReads++;	
				block = allocate_block(virtual_addr);
				lastAddr = virtual_addr/bsize;
				lastBlock = block;
				current_pte->physical_tag = block->physical_tag;
				current_pte->cacheable = block->cacheable;
				current_pte->bufferable = block->bufferable;
				return current_pte;
			}				
		}
		
		
		template < uint32_t bsize >
		
		struct tlb_block *rcache_tlb< bsize >::allocate_block(uint32_t virtual_addr)
		{
			uint32_t ind = cache_index(virtual_addr), tag = cache_tag(virtual_addr);
			tlb_block *theblock = &data[ind][choose_round_robin(ind)];
			emulator::memory_page_table_entry_t *pte = mem.get_page(virtual_addr);
			theblock->virtual_tag = tag;
			theblock->valid = true;
			theblock->cacheable = pte->cacheable;
			theblock->bufferable = pte->bufferable;
			theblock->physical_tag = pte->physical_addr/bsize;
			update_round_robin(ind);
			return theblock;
		}
		
		template rcache_tlb< 4096 >;
		
