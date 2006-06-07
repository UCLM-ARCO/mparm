/***************************************************************************
                          Memory.h  -  description
                             -------------------
    begin                : Thu May 10 2001
    copyright            : (C) 2001 Universite Paris Sud and CEA
    author               : Gilles Mouchard
    email                : gilles.mouchard@lri.fr, gilles.mouchard@.cea.fr
 ***************************************************************************/

#ifndef __BUSINTERFACEUNIT_H__
#define __BUSINTERFACEUNIT_H__


#include <systemc.h>
#include <ppc_common.h>

/* Note : This is not really a bus interface. It simulate only memory accesses ! */

const int PrimaryMemoryHashTableSize = 4096;
const int SecondaryMemoryHashTableSize = 4;
const UInt32 MemoryPageSize = 4096;

struct MemoryPageTableEntry
{
	UInt32 addr;
	struct MemoryPageTableEntry *next;
	UInt8 *storage;
};

struct SecondaryMemoryHashTable
{
	MemoryPageTableEntry *pte[SecondaryMemoryHashTableSize];
};

SC_MODULE(BusInterfaceUnit)
{
	sc_in<bool> inClock;

	/* From Data Cache */
	sc_in<bool> inDCacheReq; // Requ�e du cache de donnee
	sc_in<bool> inDCacheWrite; // Ecriture/Lecture
	sc_in<UInt32> inDCacheAddr; // Adresse de la donn� ��rire
	sc_in<DataArray<dcache2biu> > inDCacheData; // Donn� ��rire en m�oire

	/* From Instruction cache */
	sc_in<bool> inICacheReq;	// Requete du cache d'instruction
	sc_in<UInt32> inICacheAddr;	// Addresse des instructions a lire

	/* To Data Cache */
	sc_out<bool> outDCacheAck; // Store en cours/Load termine
	sc_out<DataArray<dcache2biu> > outDCacheData; // Donn�s en provenance de la m�oire

	/* To Instruction Cache */
	sc_out<bool> outICacheAck; // fetch termine
	sc_out<DataArray<icache2biu> > outICacheData; // instruction fetchee

	/* To Bus Master */             
	sc_inout<PINOUT> pinout; /*signal for amba bus master*/
	sc_in<bool> ready_from_master;
	sc_out<bool> request_to_master;

	PINOUT pinout_mast;
	//sc_uint<32> buffer[2];
	int latency;
	bool received_ack;
	int burstcounter;
	int state;
	UInt8 *memoryPtr;
	UInt32 offset;
	UInt32 addr;
	int length;
	DataArray<icache2biu> datar;
	DataArray<icache2biu> dataw;

        UInt8* startaddr; /*** indirizzo iniziale fisico della macchina come puntatore***/
	UInt32 sa; /*** indirizzo iniziale fisico della macchina come variabile intera a 32 ***/
	UInt32 count;
        void InitializeStartAddr(UInt8* addr);
        UInt8* GenerateValidAddress();

	SecondaryMemoryHashTable *primaryHashTable[PrimaryMemoryHashTableSize];

	UInt32 hash1(UInt32 addr);
	UInt32 hash2(UInt32 addr);
	void InitializeHashTable();
	MemoryPageTableEntry *AllocatePage(UInt32 addr);
	MemoryPageTableEntry *SearchPage(UInt32 addr);
	void ZeroMemory(UInt32 addr, UInt32 size);
	void MemoryWrite(UInt32 addr, UInt8 *buffer, UInt32 size);
	void MemoryRead(UInt8 *buffer, UInt32 addr, UInt32 size);
	void MemorySet(UInt32 addr, UInt8 value, UInt32 size);
	void WriteWord(UInt32 addr, UInt32 value);
	UInt64 ReadDWord(UInt32 addr);
	void WriteDWord(UInt32 addr, UInt64 value);
	UInt32 ReadWord(UInt32 addr);
	void WriteHalfWord(UInt32 addr, UInt16 value);
	UInt16 ReadHalfWord(UInt32 addr);
	void WriteByte(UInt32 addr, UInt8 value);
	UInt8 ReadByte(UInt32 addr);
	UInt32 AlignToPageBoundary(UInt32 addr);

	void Async();
	void OnFrontEdge();
	void OnFallingEdge();

	void GetRequest();

	void Reset();

	UInt32 AlignToLineBoundary(UInt32 addr);
	UInt8 *GetStorage(UInt32 addr);

	UInt64 busActivity;

	SC_CTOR(BusInterfaceUnit)
	{

		InitializeHashTable();

		count = 0;
		
		SC_METHOD(Async);
		sensitive << inDCacheReq << inICacheReq;

		SC_METHOD(OnFrontEdge);
		sensitive_pos << inClock;

		SC_METHOD(OnFallingEdge);
		sensitive_neg << inClock;

		Reset();
	}

};

#endif
