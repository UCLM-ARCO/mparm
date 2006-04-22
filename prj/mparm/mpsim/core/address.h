///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         address.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Performs address management and translation
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include <systemc.h>
#include "config.h"
#include "globals.h"
#include "mem_class.h" 

struct slv_config
{
     uint32_t firstaddr;
     uint32_t lastaddr;

     inline bool operator == (const slv_config &rhs) const
     {
        return (rhs.firstaddr == firstaddr && rhs.lastaddr == lastaddr);
     }
};



class Addresser
{
  public:
    Addresser();
    ~Addresser() {};

    uint32_t Logical2Physical(uint32_t address, uint8_t ID);
    uint32_t Physical2Logical(uint32_t address, uint8_t ID);
    uint32_t Logical2Base(uint32_t address);
    uint32_t Logical2PartScratch(uint8_t ID, uint32_t address);
    bool LogicalIsCacheable(uint32_t address);
    
    bool IsPrivate(uint16_t ID);
    bool IsLXPrivate(uint16_t ID);
    bool IsShared(uint16_t ID);
    bool IsSemaphore(uint16_t ID);
    bool IsInterrupt(uint16_t ID);
    bool IsCoreSlave(uint16_t ID);
    bool IsStorage(uint16_t ID);
    bool IsFreq(uint16_t ID);
    bool IsFFT(uint16_t ID);
    bool IsSmartmem(uint16_t ID);
    bool IsDRAM(uint16_t ID); 
    
    uint16_t PrivateStartID();
    uint16_t PrivateLXStartID();
    uint16_t SharedStartID();
    uint16_t SemaphoreStartID();
    uint16_t InterruptStartID();
    uint16_t CoreSlaveStartID();
    uint16_t StorageStartID();
    uint16_t FreqStartID();
    uint16_t Smartmem_slave_StartID();
    uint16_t FFT_slave_StartID();
    uint16_t Smartmem_master_StartID();
    uint16_t FFT_master_StartID();
    uint16_t DRAM_StartID();
    
    bool PhysicalInPrivateSpace(uint32_t address);
    bool PhysicalInPrivateLXSpace(uint32_t address);
    bool PhysicalInSharedSpace(uint32_t address);
    bool PhysicalInSemaphoreSpace(uint32_t address);
    bool PhysicalInInterruptSpace(uint32_t address);
    bool PhysicalInCoreSlaveSpace(uint32_t address);
    bool PhysicalInStorageSpace(uint32_t address);
    bool PhysicalInFreqSpace(uint32_t address);
    bool PhysicalInFFTSpace(uint32_t address);
    bool PhysicalInSmartmem(uint32_t address);
    bool PhysicalInmySmartmem(uint8_t ID,uint32_t address);
    bool PhysicalInScratchSpace(uint8_t ID, uint32_t address, uint8_t *rangeid);
    bool PhysicalInIScratchSpace(uint8_t ID, uint32_t address);
    bool PhysicalInQueueSpace(uint8_t ID, uint32_t address);
    uint8_t ReturnRangeNumber(uint8_t ID);
    void ReturnRangeBounds(uint8_t ID, uint8_t rangeid, uint32_t *begin, uint32_t *end);
    bool PhysicalInDMASpace(uint32_t address);
    bool PhysicalInInternalSpace(uint32_t address);
    bool PhysicalInSimSupportSpace(uint32_t address);
    bool PhysicalInDRAMSpace(uint32_t address);
     
    short int MapPhysicalToSlave(uint32_t address);
    
    uint32_t ReturnSlavePhysicalAddress(uint16_t ID);
    uint32_t ReturnDMAPhysicalAddress();
    uint32_t ReturnScratchPhysicalAddress(uint16_t ID);
    uint32_t ReturnIScratchPhysicalAddress(uint16_t ID);
    uint32_t ReturnQueuePhysicalAddress(uint16_t ID);
    uint32_t ReturnSimSupportPhysicalAddress();
    uint32_t ReturnDRAMPhysicalAddress();
    
    uint32_t ReturnPrivateSize();
    uint32_t ReturnPrivateLXSize();
    uint32_t ReturnSharedSize(uint16_t ID);
    uint32_t ReturnSemaphoreSize(uint16_t ID);
    uint32_t ReturnInterruptSize();
    uint32_t ReturnCoreSlaveSize();
    uint32_t ReturnStorageSize();
    uint32_t ReturnFreqSize();
    uint32_t ReturnFFTSize();
    uint32_t ReturnScratchSize();
    uint32_t ReturnIScratchSize();
    uint32_t ReturnQueueSize();
    uint32_t ReturnSlaveSize(uint16_t ID);
    uint32_t ReturnSimSupportSize();
    uint32_t ReturnDRAMSize();

  public:
    char **pMemoryDebug;
    char **scratchMemoryDebug;   // only used if we have scratchpads and the DEBUGGER
    
    Mem_class **pMem_classDebug;
    
    // Table with address ranges of all slaves
    slv_config *SLV_TABLE;
    // Every core can see the scratch at the same or at a different physical address
    // For that reason we have a different, dynamically instantiated, pointer for every core 
    slv_config *SCRATCHadd;
    // Instruction scratchpads 
    slv_config *ISCRATCHadd;
    slv_config *QUEUEadd;
    // Only necessary if DMAs will be instantiated
    slv_config DMAadd;
    // Marchal: Only needed when smart memories are used
    slv_config* SMARTMEM_DMAadd;
 
 private:
    FILE **fcheckscratch;                      // for SPCHECK
    char checkname[100];                       // for SPCHECK
    bool *tocheck;                             // for SPCHECK
    unsigned short int *nRanges;               // for SPCHECK
    unsigned int **start, **finish, **decode;  // for SPCHECK
};



// Most of these member functions are very short, so better to have them as inline


///////////////////////////////////////////////////////////////////////////////
// Logical2Physical - Converts logical addresses (as seen by processors) to
//                    physical addresses (as seen by bus).
inline uint32_t Addresser::Logical2Physical(uint32_t address, uint8_t ID)
{
  if (ID >= N_MASTERS)
  {
    printf("Fatal error: Request to translate address from device %hu, which was not supposed to exist!\n", ID);
    exit(1);
  }
  
  if (ID >= N_CORES && ID <= (N_CORES*2))                         // This is a DMA master
    ID -= N_CORES;
  
  if (address < (uint32_t)PRIVATE_SIZE && ID<N_CORES)      // Private memory access
    address += PRIVATE_SPACING * ID;                       // Mapping in the global memory space

  return address;
}

///////////////////////////////////////////////////////////////////////////////
// Physical2Logical - Converts physical addresses (as seen by bus) to
//                    logical addresses (as seen by processors).
inline uint32_t Addresser::Physical2Logical(uint32_t address, uint8_t ID)
{
  if ( (address >= (uint32_t)(PRIVATE_SPACING * ID)) && (address < (uint32_t)(PRIVATE_SPACING * ID + PRIVATE_SIZE)) )
    address -= PRIVATE_SPACING * ID;      // Private memory access
                                          // Mapping in the private memory space
  return address;
}

///////////////////////////////////////////////////////////////////////////////
// Logical2Base - Converts logical addresses to base addresses inside of the
//                referred devices.
inline uint32_t Addresser::Logical2Base(uint32_t address)
{
  uint16_t i;
  
  for (i=N_PRIVATE; i<N_SLAVES; i++)
    if (address >= SLV_TABLE[i].firstaddr && address < SLV_TABLE[i].lastaddr)
      return (address - SLV_TABLE[i].firstaddr);
      
  //FIXME:serve? Chiedere a Federico & Mirco
  #if 0    
  for (i=0;i<N_CORES;i++)
  if (SCRATCH && (address >= SCRATCHadd[i].firstaddr && address < SCRATCHadd[i].lastaddr))
    return (address - SCRATCHadd[i].firstaddr);
  #endif 
  if (DMA && (address >= DMAadd.firstaddr && address < DMAadd.lastaddr))
    return (address - DMAadd.firstaddr);
  
  return address;   // It had to be a private memory address, so if it is in
                    // the logical form, it already is in the base form too
}

///////////////////////////////////////////////////////////////////////////////
// Logical2PartScratch - Converts logical addresses to base addresses
//                       inside of a partitioned (SPCHECK) scratchpad memory.
inline uint32_t Addresser::Logical2PartScratch(uint8_t ID, uint32_t address)
{
  unsigned short int loop;
  
  if (ID >= N_CORES)
  {
    printf("Fatal error: Request to translate address from scratch %hu, which was not supposed to exist!\n", ID);
    exit(1);
  }

  if (!SPCHECK || !tocheck[ID])
    {
     return (address - SCRATCHadd[ID].firstaddr);
    }
  else
  {
    for (loop = 0; loop < nRanges[ID]; loop++)
      if (address >= start[ID][loop] && address < finish[ID][loop])
        return (decode[ID][loop] + (address - start[ID][loop]));

    // If we get here, we found no match!
    printf("Fatal error: Request to translate address from scratch %hu, but address does not belong to it!\n", ID);
    exit(1);
  }
}

///////////////////////////////////////////////////////////////////////////////
// LogicalIsCacheable - Returns true if the (logical) address is cacheable.
inline bool Addresser::LogicalIsCacheable(uint32_t address)
{
  if (address < PRIVATE_SIZE)
    return true;

#if 0    
  if (PhysicalInSmartmem(address)) 
    return true;  
#endif

#ifndef NOSNOOP
  if ( (SNOOPING) && 
     ( (address >= SHARED_BASE) && (address < SHARED_BASE+SHARED_SIZE) ) )
      return true;
#endif

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// IsPrivate - Returns true if the slave ID belongs to a private memory slave.
inline bool Addresser::IsPrivate(uint16_t ID)
{
  if (ID >= PrivateStartID() && ID < PrivateStartID() + N_PRIVATE)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// IsLXPrivate - Returns true if the slave ID belongs to a private memory slave.
inline bool Addresser::IsLXPrivate(uint16_t ID)
{
  if (ID >= PrivateLXStartID() && ID < PrivateLXStartID() + N_LX_PRIVATE)
    return true;
  else
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// IsShared - Returns true if the slave ID belongs to a shared memory slave.
inline bool Addresser::IsShared(uint16_t ID)
{
  if (ID >= SharedStartID() && ID < SharedStartID() + N_SHARED)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// IsSemaphore - Returns true if the slave ID belongs to a semaphore memory slave.
inline bool Addresser::IsSemaphore(uint16_t ID)
{
  if (ID >= SemaphoreStartID() && ID < SemaphoreStartID() + N_SEMAPHORE)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// IsInterrupt - Returns true if the slave ID belongs to an interrupt slave.
inline bool Addresser::IsInterrupt(uint16_t ID)
{
  if (ID >= InterruptStartID() && ID < InterruptStartID() + N_INTERRUPT )
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// IsCoreSlave - Returns true if the slave ID belongs to a core-associated slave.
inline bool Addresser::IsCoreSlave(uint16_t ID)
{
  if (ID >= CoreSlaveStartID() && ID < CoreSlaveStartID() + N_CORESLAVE)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// IsStorage - Returns true if the slave ID belongs to a storage slave.
inline bool Addresser::IsStorage(uint16_t ID)
{
  if (ID >= StorageStartID() && ID < StorageStartID() + N_STORAGE)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// IsFreq - Returns true if the slave ID belongs to a frequency scaling slave.
inline bool Addresser::IsFreq(uint16_t ID)
{
  if (ID >= FreqStartID() && ID < FreqStartID() + N_FREQ)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// IsFFT - Returns true if the slave ID belongs to a FFT slave.
inline bool Addresser::IsFFT(uint16_t ID)
{
  if (ID >= FFT_slave_StartID() && ID < FFT_slave_StartID() + N_FFT)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// IsSmartmem - Returns true if the slave ID belongs to a Smartmem slave.
inline bool Addresser::IsSmartmem(uint16_t ID)
{
  if (ID >= Smartmem_slave_StartID() && ID < Smartmem_slave_StartID() + N_SMARTMEM)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// IsDRAM - Returns true if the slave ID belongs to a DRAM slave.
inline bool Addresser::IsDRAM(uint16_t ID)
{
  if (ID >= DRAM_StartID() && ID < DRAM_StartID() + 1)  //FIXME N_DRAM someday
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PrivateStartID - Returns the start ID of private memory slaves.
inline uint16_t Addresser::PrivateStartID()
{
  return 0; 
}

///////////////////////////////////////////////////////////////////////////////
// PrivateLXStartID - Returns the start ID of private LX memory slaves.
inline uint16_t Addresser::PrivateLXStartID()
{
  return N_PRIVATE; 
}

///////////////////////////////////////////////////////////////////////////////
// SharedStartID - Returns the start ID of shared memory slaves.
inline uint16_t Addresser::SharedStartID()
{
  return N_PRIVATE + N_LX_PRIVATE; 
}

///////////////////////////////////////////////////////////////////////////////
// SemaphoreStartID - Returns the start ID of semaphore slaves.
inline uint16_t Addresser::SemaphoreStartID()
{
  return N_PRIVATE + N_LX_PRIVATE + N_SHARED; 
}

///////////////////////////////////////////////////////////////////////////////
// InterruptStartID - Returns the start ID of interrupt slaves.
inline uint16_t Addresser::InterruptStartID()
{
  return N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE; 
}

///////////////////////////////////////////////////////////////////////////////
// CoreSlaveStartID - Returns the start ID of core-associated slaves.
inline uint16_t Addresser::CoreSlaveStartID()
{
  return N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT; 
}

///////////////////////////////////////////////////////////////////////////////
// StorageStartID - Returns the start ID of the core-associated slaves.
inline uint16_t Addresser::StorageStartID()
{
  return N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT + N_CORESLAVE; 
}

///////////////////////////////////////////////////////////////////////////////
// FreqStartID - Returns the start ID of the core-associated slave.
inline uint16_t Addresser::FreqStartID()
{
  return N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT + N_CORESLAVE + N_STORAGE; 
}

///////////////////////////////////////////////////////////////////////////////
// Smartmem_slave_StartID - Returns the start ID of smartmem slaves.
inline uint16_t Addresser::Smartmem_slave_StartID()
{
  return N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT + N_CORESLAVE + N_STORAGE + N_FREQ; 
}

///////////////////////////////////////////////////////////////////////////////
// FFT_slave_StartID - Returns the start ID of FFT slaves.
inline uint16_t Addresser::FFT_slave_StartID()
{
  return N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT + N_CORESLAVE + N_STORAGE + N_FREQ + N_SMARTMEM; 
}

///////////////////////////////////////////////////////////////////////////////
// DRAM_StartID - Returns the start ID of DRAM slaves.
inline uint16_t Addresser::DRAM_StartID()
{
  return N_PRIVATE + N_LX_PRIVATE + N_SHARED + N_SEMAPHORE + N_INTERRUPT + N_CORESLAVE + N_STORAGE + N_FREQ + N_SMARTMEM + N_FFT; 
}

///////////////////////////////////////////////////////////////////////////////
// Smartmem_master_StartID - Returns the start ID of smartmem masters.
inline uint16_t Addresser::Smartmem_master_StartID()
{
  return N_CORES + DMA * N_CORES;
}

///////////////////////////////////////////////////////////////////////////////
// FFT_master_StartID - Returns the start ID of FFT masters.
inline uint16_t Addresser::FFT_master_StartID()
{
  return N_CORES + DMA * N_CORES + N_SMARTMEM;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInPrivateSpace - Returns true if the (physical) address belongs to the
//                          private memory space.
inline bool Addresser::PhysicalInPrivateSpace(uint32_t address)
{
  short int ID;
  
  ID = MapPhysicalToSlave(address);
  if (ID >= 0 && IsPrivate(ID))
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInPrivateLXSpace - Returns true if the (physical) address belongs to the
//                          private LX memory space.
inline bool Addresser::PhysicalInPrivateLXSpace(uint32_t address)
{
  short int ID;
  
  ID = MapPhysicalToSlave(address);
  if (ID >= 0 && IsLXPrivate(ID))
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInSharedSpace - Returns true if the (physical) address belongs to the
//                         shared memory space.
inline bool Addresser::PhysicalInSharedSpace(uint32_t address)
{
  short int ID;
  
  ID = MapPhysicalToSlave(address);
  if (ID >= 0 && IsShared(ID))
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInSemaphoreSpace - Returns true if the (physical) address belongs to
//                            the semaphore memory space.
inline bool Addresser::PhysicalInSemaphoreSpace(uint32_t address)
{
  short int ID;
  
  ID = MapPhysicalToSlave(address);
  if (ID >= 0 && IsSemaphore(ID))
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInInterruptSpace - Returns true if the (physical) address belongs to
//                            the interrupt memory space.
inline bool Addresser::PhysicalInInterruptSpace(uint32_t address)
{
  short int ID;
  
  ID = MapPhysicalToSlave(address);
  if (ID >= 0 && IsInterrupt(ID))
    return true;
  else
    return false;
}

//FIXME
///////////////////////////////////////////////////////////////////////////////
// PhysicalInCoreSlaveSpace - Returns true if the (physical) address belongs to
//                            the core-associated memory space.
inline bool Addresser::PhysicalInCoreSlaveSpace(uint32_t address)
{
  short int ID;
  
  ID = MapPhysicalToSlave(address);
  if (ID >= 0 && IsCoreSlave(ID))
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInStorageSpace - Returns true if the (physical) address belongs to
//                          the storage memory space.
inline bool Addresser::PhysicalInStorageSpace(uint32_t address)
{
  short int ID;
  
  ID = MapPhysicalToSlave(address);
  if (ID >= 0 && IsStorage(ID))
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInFreqSpace - Returns true if the (physical) address belongs to
//                       the frequency scaling memory space.
inline bool Addresser::PhysicalInFreqSpace(uint32_t address)
{
  short int ID;
  
  ID = MapPhysicalToSlave(address);
  if (ID >= 0 && IsFreq(ID))
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInFFTSpace - Returns true if the (physical) address belongs to
//                      the FFT memory space.
inline bool Addresser::PhysicalInFFTSpace(uint32_t address)
{
  short int ID;
  
  ID = MapPhysicalToSlave(address);
  if (ID >= 0 && IsFFT(ID))
    return true;
  else
    return false;
}

//marchal
///////////////////////////////////////////////////////////////////////////////
// PhysicalInSmartmem - Returns true if the (physical) address belongs to
//                       the smartmem memory space.
inline bool Addresser::PhysicalInSmartmem(uint32_t address)
{
  short int ID;
  
  ID = MapPhysicalToSlave(address);
  if (ID >= 0 && IsSmartmem(ID))
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInSmartmem - Returns true if the (physical) address belongs to
//                       the smartmem memory space.
inline bool Addresser::PhysicalInmySmartmem(uint8_t ID,uint32_t address)
{
  if (address >= SLV_TABLE[ID].firstaddr && address <= SLV_TABLE[ID].lastaddr)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInScratchSpace - Returns true if the (physical) address belongs to
//                          the scratchpad memory space mapped to the ID core.
//                          Additionally, if the scratchpad is of the partitioned
//                          type (SPCHECK active), it sets a range ID.
inline bool Addresser::PhysicalInScratchSpace(uint8_t ID, uint32_t address, uint8_t *rangeid)
{
  unsigned short int loop;
  
  if (!SCRATCH)                         // No scratchpad at all
    return false;
    
  if (ID >= N_CORES)
  {
    printf("Fatal error: Request to check address against scratchpad %hu, which was not supposed to exist!\n", ID);
    exit(1);
  }
  
  if (!SPCHECK || !tocheck[ID])         // Single-range scratchpad
    if (address >= SCRATCHadd[ID].firstaddr && address < SCRATCHadd[ID].lastaddr)
      return true;
    else
      return false;
  else                                  // Partitioned scratchpad
    for (loop = 0; loop < nRanges[ID]; loop++)
      if (address >= start[ID][loop] && address < finish[ID][loop])
      {
        *rangeid = loop;
        return true;
      }

  // We searched in a partitioned SPM, but no match was found
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInIScratchSpace - Returns true if the (physical) address belongs to
//                           the instruction scratchpad memory space mapped to
//                           the ID core.
inline bool Addresser::PhysicalInIScratchSpace(uint8_t ID, uint32_t address)
{
  if (!ISCRATCH)                         // No scratchpad at all
    return false;
    
  if (ID >= N_CORES)
  {
    printf("Fatal error: Request to check address against instruction scratchpad %hu, which was not supposed to exist!\n", ID);
    exit(1);
  }
  
  if (address >= ISCRATCHadd[ID].firstaddr && address < ISCRATCHadd[ID].lastaddr)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInQueueSpace -   Returns true if the (physical) address belongs to
//                          the queue memory space mapped to the ID core.
inline bool Addresser::PhysicalInQueueSpace(uint8_t ID, uint32_t address)
{ 
  if (!CORESLAVE)                         // No queue device at all
    return false; 
  if (ID >= N_CORESLAVE)
  {
    printf("Fatal error: Request to check address against queue %hu, which was not supposed to exist!\n", ID);
    exit(1);
  }
  
  if (address >= QUEUEadd[ID].firstaddr && address < QUEUEadd[ID].lastaddr)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnRangeNumber - Returns the number of ranges belonging to a partitioned
//                     scratchpad memory.
inline uint8_t Addresser::ReturnRangeNumber(uint8_t ID)
{
  if (ID >= N_CORES)
  {
    printf("Fatal error: Request for number of ranges of scratchpad %hu, which was not supposed to exist!\n", ID);
    exit(1);
  }
  
  if (SCRATCH && SPCHECK && tocheck[ID])
    return nRanges[ID];
  else
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnRangeBounds - Gets the boundaries of any range belonging to a partitioned
//                     scratchpad memory.
inline void Addresser::ReturnRangeBounds(uint8_t ID, uint8_t rangeid, uint32_t *begin, uint32_t *end)
{
  if (ID >= N_CORES)
  {
    printf("Fatal error: Request for range boundaries of scratchpad %hu, which was not supposed to exist!\n", ID);
    exit(1);
  }

  if (SCRATCH && SPCHECK && tocheck[ID])
  {
    if (rangeid >= nRanges[ID])
    {
      printf("Fatal error: Request for boundaries of range %hu of scratchpad %hu, which does not exist!\n", rangeid, ID);
      exit(1);
    }
    *begin = start[ID][rangeid];
    *end = finish[ID][rangeid];
  }
  else    // Some other error condition!
  {
    printf("Fatal error: Cannot return boundaries for the range %hu of scratchpad %hu!\n", rangeid, ID);
    exit(1);
  }
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInDMASpace - Returns true if the (physical) address belongs to the
//                     DMA memory space.
inline bool Addresser::PhysicalInDMASpace(uint32_t address)
{
  if (DMA && (address >= DMAadd.firstaddr && address < DMAadd.lastaddr))
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInInternalSpace - Returns true if the (physical) address belongs
//                           to the internal devices memory space.
inline bool Addresser::PhysicalInInternalSpace(uint32_t address)
{
  if (address >= 0x80000000)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInSimSupportSpace - Returns true if the (physical) address belongs
//                             to the simulation support device memory space.
inline bool Addresser::PhysicalInSimSupportSpace(uint32_t address)
{
  if (address >= SIMSUPPORT_BASE && address < SIMSUPPORT_BASE + SIMSUPPORT_SIZE)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// PhysicalInDRAMSpace - Returns true if the (physical) address belongs
//                       to the DRAM device memory space.
inline bool Addresser::PhysicalInDRAMSpace(uint32_t address)
{
  if (!DRAM)
    return false;
  
  if (address >= SLV_TABLE[DRAM_StartID()].firstaddr && address < SLV_TABLE[DRAM_StartID()].lastaddr)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// MapPhysicalToSlave - Maps a (physical) address to the corresponding slave.
//                      Returns -1 if the address belongs to DMA or scratchpad
//                      devices (if present), exits if no mapping can be found.
inline short int Addresser::MapPhysicalToSlave(uint32_t address)
{
  uint8_t i, dummy;

  for (i = 0; i < N_SLAVES; i ++)
    if (address >= SLV_TABLE[i].firstaddr && address < SLV_TABLE[i].lastaddr)
      return i;

  // If SPCHECK is active, we'd need to know who is asserting this request.
  // Otherwise, the call to PhysicalInScratchSpace cannot know what
  // range of addresses to check. So, skip this case.
  if (SCRATCH && !SPCHECK)
    for (i = 0; i < N_CORES; i ++)
      if (PhysicalInScratchSpace(i, address, &dummy))
        return -1;

  if (DMA && (PhysicalInDMASpace(address)))
    return -1;
    
  // If we are here, two things might have happened: either SPCHECK was on
  // and we had not enough input to check inside of partitioned SPM ranges,
  // or the address was invalid. If SPCHECK is on, assume that we may have
  // caught an SPM access and return -1 (this special value should be anyway
  // carefully managed by the caller). Otherwise, exit now with an error!
  if (SPCHECK)
    return -1;

  // Error condition!
  printf("Fatal error: Address 0x%08x cannot be mapped to any existing slave, nor (if present) to DMA/scratchpad!\n", address);
  exit(1);
  return(0);  // Dummy return
}

///////////////////////////////////////////////////////////////////////////////
// ReturnSlavePhysicalAddress - Returns the base physical address of the
//                              selected slave.
inline uint32_t Addresser::ReturnSlavePhysicalAddress(uint16_t ID)
{
  if (ID < N_SLAVES)
    return SLV_TABLE[ID].firstaddr;
  else  // Error condition!
  {
    printf("Fatal error: Mapping slave %u to any memory address is impossible!\n", ID);
    exit(1);
    return(0);  // Dummy return
  }
}

///////////////////////////////////////////////////////////////////////////////
// ReturnDMAPhysicalAddress - Returns the base physical address of the DMA
//                            device (if present).
inline uint32_t Addresser::ReturnDMAPhysicalAddress()
{
  if (DMA)
    return DMAadd.firstaddr;
  else
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnScratchPhysicalAddress - Returns the base physical address of the
//                                selected scratchpad device (if present).
inline uint32_t Addresser::ReturnScratchPhysicalAddress(uint16_t ID)
{
  if (SCRATCH)
    return SCRATCHadd[ID].firstaddr;
  else
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnIScratchPhysicalAddress - Returns the base physical address of the
//                                 selected instruction scratchpad device (if
//                                 present).
inline uint32_t Addresser::ReturnIScratchPhysicalAddress(uint16_t ID)
{
  if (ISCRATCH)
    return ISCRATCHadd[ID].firstaddr;
  else
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnQueuePhysicalAddress - Returns the base physical address of the
//                              selected queue device (if present).
inline uint32_t Addresser::ReturnQueuePhysicalAddress(uint16_t ID)
{
  if (CORESLAVE)
    return QUEUEadd[ID].firstaddr;
  else
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnSimSupportPhysicalAddress - Returns the base physical address of the
//                                   simulation support device.
inline uint32_t Addresser::ReturnSimSupportPhysicalAddress()
{
  return SIMSUPPORT_BASE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnDRAMPhysicalAddress - Returns the base physical address of the
//                             DRAM device.
inline uint32_t Addresser::ReturnDRAMPhysicalAddress()
{
  return SLV_TABLE[DRAM_StartID()].firstaddr;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnPrivateSize - Returns the size of a private memory.
inline uint32_t Addresser::ReturnPrivateSize()
{
  return PRIVATE_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnPrivateLXSize - Returns the size of a private LX memory.
inline uint32_t Addresser::ReturnPrivateLXSize()
{
  return PRIVATE_LX_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnSharedSize - Returns the size of a shared memory.
//                    FIXME - no internal check, use IsShared(ID) first or maybe ReturnSlaveSize()
inline uint32_t Addresser::ReturnSharedSize(uint16_t ID)
{
  return SLV_TABLE[ID].lastaddr - SLV_TABLE[ID].firstaddr;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnSemaphoreSize - Returns the size of a semaphore memory.
//                       FIXME - no internal check, use IsSemaphore(ID) first or maybe ReturnSlaveSize()
inline uint32_t Addresser::ReturnSemaphoreSize(uint16_t ID)
{
  return SLV_TABLE[ID].lastaddr - SLV_TABLE[ID].firstaddr;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnInterruptSize - Returns the size of an interrupt slave.
inline uint32_t Addresser::ReturnInterruptSize()
{
  return INTERRUPT_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnCoreSlaveSize - Returns the size of a core-associated slave.
inline uint32_t Addresser::ReturnCoreSlaveSize()
{
  return CORESLAVE_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnStorageSize - Returns the size of a storage slave.
inline uint32_t Addresser::ReturnStorageSize()
{
  return STORAGE_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnFreqSize - Returns the size of a frequency scaling slave.
inline uint32_t Addresser::ReturnFreqSize()
{
  return FREQ_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnFFTSize - Returns the size of a FFT slave.
inline uint32_t Addresser::ReturnFFTSize()
{
  return FFT_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnScratchSize - Returns the size of a scratchpad memory.
inline uint32_t Addresser::ReturnScratchSize()
{
  return SCRATCH_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnIScratchSize - Returns the size of an instruction scratchpad memory.
inline uint32_t Addresser::ReturnIScratchSize()
{
  return ISCRATCHSIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnScratchSize - Returns the size of a queue device.
inline uint32_t Addresser::ReturnQueueSize()
{
  return QUEUE_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnSimSupportSize - Returns the size of the simulation support device.
inline uint32_t Addresser::ReturnSimSupportSize()
{
  return SIMSUPPORT_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnDRAMSize - Returns the size of the DRAM device.
inline uint32_t Addresser::ReturnDRAMSize()
{
  return DRAMDMA_SIZE;
}

///////////////////////////////////////////////////////////////////////////////
// ReturnSlaveSize - Returns the size of the selected slave.
inline uint32_t Addresser::ReturnSlaveSize(uint16_t ID)
{
  if (IsPrivate(ID))
    return ReturnPrivateSize();
  if (IsShared(ID))
    return ReturnSharedSize(ID);
  if (IsSemaphore(ID))
    return ReturnSemaphoreSize(ID);
  if (IsInterrupt(ID))
    return ReturnInterruptSize();
  if (IsCoreSlave(ID))
    return ReturnCoreSlaveSize();
  if (IsStorage(ID))
    return ReturnStorageSize();
  if (IsFreq(ID))
    return ReturnFreqSize();
  if (IsDRAM(ID))
    return ReturnDRAMSize();

  // Error condition!
  printf("Fatal error: Gettimg the size of slave %u is impossible!\n", ID);
  exit(1);
  return(0);  // Dummy return
}

extern Addresser *addresser;

#endif // __ADDRESS_H__
