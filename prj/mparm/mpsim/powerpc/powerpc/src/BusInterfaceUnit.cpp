/***************************************************************************
                          BusInterfaceUnit.cpp  -  description
                             -------------------
    begin                : Thu May 10 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#include <BusInterfaceUnit.h>
#include <string.h>
//#define DEBUG1
//#define DEBUG2 //wb
//#define DEBUG50

UInt32 BusInterfaceUnit::AlignToLineBoundary(UInt32 addr)
{
	return addr - (addr % linesize);
}

void BusInterfaceUnit::GetRequest()
{
	if(inICacheReq)
	{
		UInt32 addr = inICacheAddr;
#ifdef DEBUG1
//		if(Debug(DebugBusInterfaceUnit))	commentata perche` appartiene a Simulator
		{
			cout << name() << ": Getting an icache request at ";
			WriteHex(cout, addr);
			cout << endl;
		}
#endif
		offset = addr % linesize;
		
		memoryPtr = GetStorage(AlignToLineBoundary(addr));
#ifdef DEBUG1			
		printf("Richiesta di lettura dalla icache sul bus all`indirizzo %x allineato alla linea di cache (funzione GetRequest)\n", 
		(int) memoryPtr);
#endif
		length = 0;
		burstcounter = 0;

		//latency = memoryReadLatency;
		busActivity += 1;

		pinout_mast.address = (UInt32) (&memoryPtr[offset]);	
		pinout_mast.rw = 0; //read
		pinout_mast.bw = 0; //dimension of 32 bit
		pinout_mast.benable = 1;
		pinout_mast.burst = 8;
 		pinout.write(pinout_mast);
		request_to_master = true;
		received_ack = false;

#ifdef DEBUG1			
		cout << name() << ": Invio richiesta di lettura sul bus all`indirizzo " <<
		pinout_mast.address << endl;
#endif
		state = 5;




		/*	if(length == linesize)
			{
				state = 0;
			}
			else
			{
				state = 6;
			}

			outICacheData = data;*/
	} else if(inDCacheReq)
	{
		UInt32 addr = inDCacheAddr;
#ifdef DEBUG50
//		if(Debug(DebugBusInterfaceUnit))	commentata perche` appartiene a Simulator
	if (sc_simulation_time()>7600000)
		{
			cout << name() << ": Getting a dcache request at ";
			WriteHex(cout, addr);
			cout << endl;
		}
#endif
		offset = addr % linesize;
		memoryPtr = GetStorage(AlignToLineBoundary(addr));
		length = 0;

		if(inDCacheWrite)
		{
			
#ifdef DEBUG2			
			printf("Buffering richiesta di scrittura sul bus all`indirizzo %x\n",
			(int) &memoryPtr[offset]);
#endif
			//latency = memoryWriteLatency;
			busActivity += 1;
			burstcounter = 0;
			//received_ack = true;
			dataw = inDCacheData.read();
			sc_bv<32> dataonbus;
			dataonbus.range(31,24)=dataw.buffer[3];
			dataonbus.range(23,16)=dataw.buffer[2];
			dataonbus.range(15,8)=dataw.buffer[1];
			dataonbus.range(7,0)=dataw.buffer[0];
			
#ifdef DEBUG2				
			cout << name() << ": Data on bus = " << dataonbus << endl;
#endif			
			
			pinout_mast.data = dataonbus.to_uint();
			pinout_mast.address = (UInt32) &memoryPtr[offset];
			pinout_mast.burst = 8;			
	
			pinout_mast.rw = 1; //write
			pinout_mast.bw = 0; //dimension of 32 bit
			pinout_mast.benable = 1;
			
#ifdef DEBUG2
			printf("%s: Writing 4 bytes (%x)\n",
			name(), pinout_mast.data);
#endif
			outDCacheAck = false;
			pinout.write(pinout_mast);
			request_to_master = true;
			burstcounter ++;

			//outDCacheAck = true;
			state = 1;
#ifdef DEBUG1
//			if(Debug(DebugBusInterfaceUnit))	commentata perche` appartiene a Simulator
				cout << name() << ": Sending ack" << endl;
#endif
		}
		else
		{
			busActivity += 1;
			burstcounter = 0;
			pinout_mast.address = (UInt32) &memoryPtr[offset];
			pinout_mast.burst = 8;			
			pinout_mast.rw = 0; //read
			pinout_mast.bw = 0; //dimension of 32 bit
			pinout_mast.benable = 1;
			pinout.write(pinout_mast);
			request_to_master=true;
			received_ack=false;
			
#ifdef DEBUG1			
			printf("Invio richiesta di lettura dalla dcache sul bus all`indirizzo %x\n",
			(int) &memoryPtr[offset]);
#endif
			state = 3;
			/*latency = memoryReadLatency;


			if(--latency > 0)
			{
				state = 3;
			}
			else
			{
				DataArray<dcache2biu> data;

				outDCacheAck = true;
				memcpy(data.buffer, &memoryPtr[offset], dcache2biu);
				offset = (offset + dcache2biu) % linesize;
				length += dcache2biu;

				if(length == linesize)
				{
					state = 0;
				}
				else
				{
					state = 4;
				}

				outDCacheData = data;
			}*/
		}
	}
}

void BusInterfaceUnit::Async()
{
	if (state == 0) GetRequest();
}

void BusInterfaceUnit::OnFrontEdge()
{
#ifdef DEBUG1
	//if(Debug(DebugBusInterfaceUnit))
		cout << name() << ": state = " << state << endl;
#endif
	if (received_ack == false){
		if (ready_from_master == true) {
			received_ack = true;
#ifdef DEBUG1				
			cout << name() << ": Receiving ready from bus..." << endl;
#endif			
		}
	}
	switch(state)
	{
		case 0:
			GetRequest();			
			break;

		case 1: // First state of the writing process
			if(received_ack == true)
			{

				sc_bv<32> dataonbus;
				dataonbus.range(31,24)=dataw.buffer[7];
				dataonbus.range(23,16)=dataw.buffer[6];
				dataonbus.range(15,8)=dataw.buffer[5];
				dataonbus.range(7,0)=dataw.buffer[4];
				
#ifdef DEBUG2				
				cout << name() << ": Data on bus = " << dataonbus << endl;
#endif				
				
				pinout_mast.data = dataonbus.to_uint();
#ifdef DEBUG2			
				printf("%s: Writing 4 bytes (%x)\n",
				name(), pinout_mast.data);
#endif				
				busActivity += 1;
				pinout.write(pinout_mast);
				outDCacheAck = true;
				burstcounter++;
				received_ack = false;
				request_to_master = false;
                                if (burstcounter < 8)
				{
					state = 2;
				}
				else
				{
					state = 7;
				}

			}
			break;

		case 2: //Second state of the writing process
			
			outDCacheAck = false;
			if(received_ack == true)
			{
#ifdef DEBUG50
//				if(Debug(DebugBusInterfaceUnit))	commentata perche` appartiene a Simulator
		if (sc_simulation_time()>7600000)
		{
				cout << name() << ": Sending ack to DCache" << endl;
		}
#endif
				dataw = inDCacheData.read();
				sc_bv<32> dataonbus;
				dataonbus.range(31,24)=dataw.buffer[3];
				dataonbus.range(23,16)=dataw.buffer[2];
				dataonbus.range(15,8)=dataw.buffer[1];
				dataonbus.range(7,0)=dataw.buffer[0];
				
#ifdef DEBUG2				
				cout << name() << ": Data on bus = " << dataonbus << endl;
#endif				
				
				pinout_mast.data = dataonbus.to_uint();
#ifdef DEBUG2
				printf("%s: Writing 4 bytes (%x)\n",
				name(), pinout_mast.data);
#endif				
				busActivity += 1;
				pinout.write(pinout_mast);
				burstcounter++;
				received_ack = false;
				request_to_master = false;
				if (burstcounter < 8)
				{
					state = 1;
				}
				else
				{
					state = 7;
				}
			}

			break;

		/*case 3:
			if(--latency == 0)
			{
#ifdef DEBUG
				if(Debug(DebugBusInterfaceUnit))
					cout << name() << ": Sending ack to DCache" << endl;
#endif
				outDCacheAck = true;

				DataArray<dcache2biu> data;

				memcpy(data.buffer, &memoryPtr[offset], dcache2biu);
				offset = (offset + dcache2biu) % linesize;
				length += dcache2biu;

				if(length == linesize)
				{
					state = 0;
				}
				else
				{
					state = 4;
				}

				outDCacheData = data;
			}
			break;

		case 4:
			{
				outDCacheAck = false;

				DataArray<dcache2biu> data;

				memcpy(data.buffer, &memoryPtr[offset], dcache2biu);
				offset = (offset + dcache2biu) % linesize;
				length += dcache2biu;

				if(length == linesize)
				{
					state = 0;
				}

				outDCacheData = data;
			}
			break;*/

		case 3:// First state of the read process of the data cache
			outDCacheAck = false;			
			if(received_ack == true)
			{
				request_to_master = false;			
				sc_bv<32> dataonbus;
				sc_bv<8> dummy;
				busActivity += 1;
				pinout_mast = pinout.read();
				dataonbus = pinout_mast.data;
				received_ack = false;
#ifdef DEBUG1				
				cout << name() << ": Reading data from bus; cycle # " << burstcounter << endl;
#endif		
#ifdef DEBUG1				
				cout << name() << ": Data on bus = " << dataonbus << endl;
#endif				
				dummy = dataonbus.range(7,0);
				
				datar.buffer[0]=dummy.to_uint();

				dummy = dataonbus.range(15,8);
				
				datar.buffer[1]=dummy.to_uint();
					
				dummy = dataonbus.range(23,16);
				
				datar.buffer[2] = dummy.to_uint();
					
				dummy = dataonbus.range(31,24);
			
				datar.buffer[3] = dummy.to_uint();
				
#ifdef DEBUG1			
				cout << name() << ": Lettura dei byte:  " << datar << endl;
#endif				

				if (burstcounter < 8)
				{
					state = 4;
				}
				else
				{
					state = 8;
				}

				burstcounter++;
				/*offset = (offset + icache2biu) % linesize;
				length += icache2biu;

				if(length == linesize)
				{
					state = 0;
				}
				else
				{
					state = 6;
				}

				outICacheData = data;*/
			}
			break;

		case 4://Second state of the read process
			if (received_ack == true)
			{
				outDCacheAck = true;
				sc_bv<32> dataonbus;
				sc_bv<8> dummy;
				busActivity += 1;
				pinout_mast = pinout.read();
				dataonbus = pinout_mast.data;
				received_ack = false;
#ifdef DEBUG1				
				cout << name() << ": Reading data from bus; cycle # " << burstcounter << endl;
#endif				
		
#ifdef DEBUG1				
				cout << name() << ": Data on bus = " << dataonbus << endl;
#endif				
				dummy = dataonbus.range(7,0);
				
				datar.buffer[4]=dummy.to_uint();

				dummy = dataonbus.range(15,8);
				
				datar.buffer[5]=dummy.to_uint();
					
				dummy = dataonbus.range(23,16);
				
				datar.buffer[6] = dummy.to_uint();
					
				dummy = dataonbus.range(31,24);
			
				datar.buffer[7] = dummy.to_uint();
				
#ifdef DEBUG1			
				cout << name() << ": Lettura dei byte:  " << datar << endl;
#endif	

				burstcounter++;
				
				//memcpy(data.buffer, &memoryPtr[offset], icache2biu);
				if (burstcounter < 8)
				{
					state = 3;
				}
				else
				{	
					state = 8;
				}

				/*offset = (offset + icache2biu) % linesize;
				length += icache2biu;

				if(length == linesize)
				{
					state = 0;
				}*/

				outDCacheData = datar;
			}
			break;
		
		case 5:// First state of the read process of the instruction cache
			outICacheAck = false;			
			if(received_ack == true)
			{
				request_to_master = false;				
				sc_bv<32> dataonbus;
				sc_bv<8> dummy;
				busActivity += 1;
				pinout_mast = pinout.read();
				dataonbus = pinout_mast.data;
				received_ack = false;
#ifdef DEBUG1				
				cout << name() << ": Reading data from bus; cycle # " << burstcounter << endl;
#endif		
#ifdef DEBUG1				
				cout << name() << ": Data on bus = " << dataonbus << endl;
#endif				
				dummy = dataonbus.range(7,0);
				
				datar.buffer[0]=dummy.to_uint();

				dummy = dataonbus.range(15,8);
				
				datar.buffer[1]=dummy.to_uint();
					
				dummy = dataonbus.range(23,16);
				
				datar.buffer[2] = dummy.to_uint();
					
				dummy = dataonbus.range(31,24);
			
				datar.buffer[3] = dummy.to_uint();
				
#ifdef DEBUG1			
				cout << name() << ": Lettura dei byte:  " << datar << endl;
#endif				

				if (burstcounter < 8)
				{
					state = 6;
				}
				else
				{
					state = 8;
				}

				burstcounter++;
				/*offset = (offset + icache2biu) % linesize;
				length += icache2biu;

				if(length == linesize)
				{
					state = 0;
				}
				else
				{
					state = 6;
				}

				outICacheData = data;*/
			}
			break;

		case 6://Second state of the read process of the instruction cache

			if (received_ack == true)
			{
				outICacheAck = true;				
				sc_bv<32> dataonbus;
				sc_bv<8> dummy;
				busActivity += 1;
				pinout_mast = pinout.read();
				dataonbus = pinout_mast.data;
				received_ack = false;
#ifdef DEBUG1				
				cout << name() << ": Reading data from bus; cycle # " << burstcounter << endl;
#endif				
		
#ifdef DEBUG1				
				cout << name() << ": Data on bus = " << dataonbus << endl;
#endif				
				dummy = dataonbus.range(7,0);
				
				datar.buffer[4]=dummy.to_uint();

				dummy = dataonbus.range(15,8);
				
				datar.buffer[5]=dummy.to_uint();
					
				dummy = dataonbus.range(23,16);
				
				datar.buffer[6] = dummy.to_uint();
					
				dummy = dataonbus.range(31,24);
			
				datar.buffer[7] = dummy.to_uint();
				
#ifdef DEBUG1			
				cout << name() << ": Lettura dei byte:  " << datar << endl;
#endif	

				burstcounter++;
				
				//memcpy(data.buffer, &memoryPtr[offset], icache2biu);
				if (burstcounter < 8)
				{
					state = 5;
				}
				else
				{
					state = 8;
				}

				/*offset = (offset + icache2biu) % linesize;
				length += icache2biu;

				if(length == linesize)
				{
					state = 0;
				}*/

				outICacheData = datar;
			}
			break;
		case 7: // end of writing process
			outDCacheAck = false;
			outICacheAck = false;
			if (received_ack == true)
			{
				received_ack = false;
				state = 0;
			
			}
			break;	
		case 8: // end of reading process
			outDCacheAck = false;
			outICacheAck = false;
			received_ack = false;
			state = 0;
			break;			
	}
}

void BusInterfaceUnit::OnFallingEdge()
{
	switch(state)
	{

		case 1: //Read Date from DCache
			if(received_ack == true)
			{

			datar = inDCacheData;
#ifdef DEBUG1
//			if(Debug(DebugBusInterfaceUnit))	commentata perche` appartiene a Simulator
//				cout << name() << ": Writing 4 bytes (" << data << ")" << endl;
#endif
			}

		/*case 2: // Write Data into memory
			{
				outDCacheAck = false;

				const DataArray<dcache2biu>& data = inDCacheData;

				memcpy(&memoryPtr[offset], data.buffer, dcache2biu);
				offset = (offset + dcache2biu) % linesize;
				length += dcache2biu;

				if(length == linesize)
				{
					state = 0;
				}
			}
			break;*/
	}
}

UInt32 BusInterfaceUnit::hash1(UInt32 addr)
{
	return (addr / MemoryPageSize) % PrimaryMemoryHashTableSize;
//	return (39 * (addr / MemoryPageSize) + 11) % PrimaryMemoryHashTableSize;
}

UInt32 BusInterfaceUnit::hash2(UInt32 addr)
{
	return (addr / MemoryPageSize / PrimaryMemoryHashTableSize) % SecondaryMemoryHashTableSize;
//	return (41 *(addr / MemoryPageSize) + 7) % SecondaryMemoryHashTableSize;
}

void BusInterfaceUnit::InitializeHashTable()
{
	int i;

	for(i = 0; i < PrimaryMemoryHashTableSize; i++)
	{
		primaryHashTable[i] = 0;
	}
}

MemoryPageTableEntry *BusInterfaceUnit::AllocatePage(UInt32 addr)
{
	addr = addr - (addr % MemoryPageSize);
	MemoryPageTableEntry *pte = SearchPage(addr);
	if(!pte)
	{
		UInt32 h1 = hash1(addr);
		SecondaryMemoryHashTable *secondaryHashTable = primaryHashTable[h1];

		if(!secondaryHashTable)
		{
			secondaryHashTable = new SecondaryMemoryHashTable;
			int i;
			for(i = 0; i < SecondaryMemoryHashTableSize; i++)
			{
				secondaryHashTable -> pte[i] = 0;
			}
			primaryHashTable[h1] = secondaryHashTable;
		}

		UInt32 h2 = hash2(addr);
		pte = new MemoryPageTableEntry;
		pte->addr = addr;
		pte->storage = GenerateValidAddress();
		memset(pte->storage + sa, 0, MemoryPageSize);
		pte->next = secondaryHashTable->pte[h2];
		secondaryHashTable->pte[h2] = pte;
	}
	return pte;
}

MemoryPageTableEntry *BusInterfaceUnit::SearchPage(UInt32 addr)
{
	addr = addr - (addr % MemoryPageSize);

	UInt32 h1 = hash1(addr);
	SecondaryMemoryHashTable *secondaryHashTable = primaryHashTable[h1];

	if(secondaryHashTable)
	{
		UInt32 h2 = hash2(addr);
		MemoryPageTableEntry *pte = secondaryHashTable->pte[h2];

		if(pte)
		{
			do
			{
				if(pte->addr == addr) return pte;
			} while((pte = pte->next) != 0);
		}
	}
	return 0;
}

void BusInterfaceUnit::ZeroMemory(UInt32 addr, UInt32 size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MemoryPageSize;
		MemoryPageTableEntry *page = AllocatePage(addr);
		UInt32 sz = MemoryPageSize - offset;
		if(size > sz)
		{
			memset(page->storage + offset + sa, 0, sz);
			size -= sz;
			addr += sz;

			if(size >= MemoryPageSize)
			{
				do
				{
					page = AllocatePage(addr);
					memset(page->storage + sa, 0, MemoryPageSize);
					size -= MemoryPageSize;
					addr += MemoryPageSize;
				} while(size >= MemoryPageSize);
			}

			if(size > 0)
			{
				page = AllocatePage(addr);
				memset(page->storage + sa, 0, size);
			}
		}
		else
		{
			memset(page->storage + offset + sa, 0, size);
		}
	}
}

void BusInterfaceUnit::MemorySet(UInt32 addr, UInt8 value, UInt32 size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MemoryPageSize;
		MemoryPageTableEntry *page = AllocatePage(addr);
		UInt32 sz = MemoryPageSize - offset;
		if(size > sz)
		{
			memset(page->storage + offset + sa, value, sz);
			size -= sz;
			addr += sz;

			if(size >= MemoryPageSize)
			{
				do
				{
					page = AllocatePage(addr);
					memset(page->storage + sa, value, MemoryPageSize);
					size -= MemoryPageSize;
					addr += MemoryPageSize;
				} while(size >= MemoryPageSize);
			}

			if(size > 0)
			{
				page = AllocatePage(addr);
				memset(page->storage + sa, value, size);
			}
		}
		else
		{
			memset(page->storage + offset + sa, value, size);
		}
	}
}

void BusInterfaceUnit::InitializeStartAddr(UInt8* addr)
{
        startaddr = addr;
	sa = (UInt32) addr;
#ifdef DEBUG1	
	printf("Indirizzo iniziale passato a BI: %x\n", 
	(int) startaddr);
#endif
}

UInt8* BusInterfaceUnit::GenerateValidAddress() //genera un indirizzo valido incrementando il
						//puntatore di count
{
	if (count * MemoryPageSize > addresser->ReturnPrivateSize())
	{
		printf("Indirizzo generato supera i limiti della memoria fisica. Contatore di pagina : %d\n", count);
	
	}

        UInt8* addr;
	addr = (UInt8*) (count * MemoryPageSize);
#ifdef DEBUG1	
        printf("Indirizzo della extram generato al passo %d: %x\n", count, (int) addr);	
	
	//printf("Conteggio indirizzi generati: %d\n", count);
#endif
	count ++;
        return addr;
}
void BusInterfaceUnit::MemoryWrite(UInt32 addr, UInt8 *buffer, UInt32 size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MemoryPageSize;
		MemoryPageTableEntry *page = AllocatePage(addr);
		UInt32 sz = MemoryPageSize - offset;
#ifdef DEBUG1		
		printf("Indirizzo virtuale del dato = %x, dimensione = %d, offset nella pagina = %x\n", addr, size, offset);		
#endif		
			
		if(size > sz)
		{
#ifdef DEBUG1
			printf("Scrittura in memoria del dato %d all'indirizzo %x\n", 
			*buffer,(int) page -> storage + offset + sa);
#endif			
			memcpy(page->storage + offset + sa, buffer, sz);
			
			size -= sz;
			addr += sz;
			buffer += sz;

			if(size >= MemoryPageSize)
			{
				do
				{
					page = AllocatePage(addr);
					memcpy(page->storage + sa, buffer, MemoryPageSize);
#ifdef DEBUG1					
					printf("Scrittura in memoria del dato %x all'indirizzo di macchina %x\n", 
					*buffer, (int) page -> storage + sa);
#endif
					size -= MemoryPageSize;
					addr += MemoryPageSize;
					buffer += MemoryPageSize;
				} while(size >= MemoryPageSize);
			}

			if(size > 0)
			{
				page = AllocatePage(addr);
				memcpy(page->storage + sa, buffer, size);
#ifdef DEBUG1				
				printf("Scrittura in memoria del dato %x all'indirizzo %x\n",
				*buffer, (int) page -> storage + sa);
#endif
			}
		}
		else
		{
			memcpy(page->storage + offset + sa, buffer, size);
#ifdef DEBUG1			
			printf("Scrittura in memoria del dato %x all'indirizzo %x\n", 
			*buffer,(int) page -> storage + offset + sa);
#endif		
		}
	}
}

void BusInterfaceUnit::MemoryRead(UInt8 *buffer, UInt32 addr, UInt32 size)
{
	if(size > 0)
	{
		UInt32 offset = addr % MemoryPageSize;
		MemoryPageTableEntry *page = AllocatePage(addr);
		UInt32 sz = MemoryPageSize - offset;
		if(size > sz)
		{
			memcpy(buffer, page->storage + offset + sa, sz);
			size -= sz;
			addr += sz;
			buffer += sz;

			if(size >= MemoryPageSize)
			{
				do
				{
					page = AllocatePage(addr);
					memcpy(buffer, page->storage + sa, MemoryPageSize);
					size -= MemoryPageSize;
					addr += MemoryPageSize;
					buffer += MemoryPageSize;
				} while(size >= MemoryPageSize);
			}

			if(size > 0)
			{
				page = AllocatePage(addr);
				memcpy(buffer, page->storage + sa, size);
			}
		}
		else
		{
			memcpy(buffer, page->storage + offset + sa, size);
		}
	}
}

void BusInterfaceUnit::WriteDWord(UInt32 addr, UInt64 value)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	pte->storage[offset + sa] = value >> 56;
	pte->storage[offset + 1 + sa] = value >> 48;
	pte->storage[offset + 2 + sa] = value >> 40;
	pte->storage[offset + 3 + sa] = value >> 32;
	pte->storage[offset + 4 + sa] = value >> 24;
	pte->storage[offset + 5 + sa] = value >> 16;
	pte->storage[offset + 6 + sa] = value >> 8;
	pte->storage[offset + 7 + sa] = value;
}

UInt64 BusInterfaceUnit::ReadDWord(UInt32 addr)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	return ((UInt64) pte->storage[offset + sa] << 56)
	     | ((UInt64) pte->storage[offset + 1 + sa] << 48)
	     | ((UInt64) pte->storage[offset + 2 + sa] << 40)
	     | ((UInt64) pte->storage[offset + 3 + sa] << 32)
	     | ((UInt64) pte->storage[offset + 4 + sa] << 24)
	     | ((UInt64) pte->storage[offset + 5 + sa] << 16)
	     | ((UInt64) pte->storage[offset + 6 + sa] << 8)
	     | ((UInt64) pte->storage[offset + 7 + sa]);
}

void BusInterfaceUnit::WriteWord(UInt32 addr, UInt32 value)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	pte->storage[offset + sa] = value >> 24;
	pte->storage[offset + 1 + sa] = value >> 16;
	pte->storage[offset + 2 + sa] = value >> 8;
	pte->storage[offset + 3 + sa] = value;
}

UInt32 BusInterfaceUnit::ReadWord(UInt32 addr)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	return (pte->storage[offset + sa] << 24) | (pte->storage[offset + 1 + sa] << 16) | (pte->storage[offset + 2 + sa] << 8) | (pte->storage[offset + 3 + sa]);
}

void BusInterfaceUnit::WriteHalfWord(UInt32 addr, UInt16 value)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	pte->storage[offset + sa] = value >> 8;
	pte->storage[offset + 1 + sa] = value;
}

UInt16 BusInterfaceUnit::ReadHalfWord(UInt32 addr)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	return (pte->storage[offset + sa] << 8) | (pte->storage[offset + 1 + sa]);
}

void BusInterfaceUnit::WriteByte(UInt32 addr, UInt8 value)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	pte->storage[offset + sa] = value;
}

UInt8 BusInterfaceUnit::ReadByte(UInt32 addr)
{
	UInt32 offset = addr % MemoryPageSize;
	MemoryPageTableEntry *pte = AllocatePage(addr);
	return pte->storage[offset + sa];
}

UInt8 *BusInterfaceUnit::GetStorage(UInt32 addr)
{
	MemoryPageTableEntry *pte = AllocatePage(addr);

	if(pte) 
	{
#ifdef DEBUG1	
	printf("\nIndirizzo fisico associato all'indirizzo virtuale %x = %x (funzione GetStorage)\n", 
	(int) addr, (int) pte->storage + (addr % MemoryPageSize));
#endif	
	return pte->storage + (addr % MemoryPageSize);
	}
	cout << name() << ": Error : address ";
	WriteHex(cout, addr);
	cout << " is not mapped" << endl;
	abort();// modificato, StopSimulation() non si usa perche` definito in Simulator.cpp
	//StopSimulation(); return 0;
	return 0;
}

UInt32 BusInterfaceUnit::AlignToPageBoundary(UInt32 addr)
{
	return addr - (addr % MemoryPageSize);
}

void BusInterfaceUnit::Reset()
{
	state = 0;
	busActivity = 0;
}
