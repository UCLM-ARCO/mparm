///////////////////////////////////////////////////////////////////////////////
// Copyright 2000 Michael Dales
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// name   armproc.h
// author Michael Dales (michael@dcs.gla.ac.uk)
// header n/a
// info   An ARM processor - glues together the cache and what have you.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ARMPROC_H__
#define __ARMPROC_H__

#include <systemc.h>
#include "swarm.h"
#include "core.h"
//#include "cache.h"
class CCache;  // Forward declaration. Don't include cache.h
#include "swi.h"
#include <iostream>
#include "copro.h"
#include "ostimer.h"
#include "intctrl.h"
#include "lcdctrl.h"
#include "uartctrl.h"
#include "config.h"
#include "globals.h"
#include "address.h"
#include "globals.h"
#include "mem_class.h"
#include "core_signal.h"

// ARM internal states
typedef enum {
  P_NORMAL
  , P_READING1
  , P_READING
  , P_WRITING1
  , P_INTWRITE
  , P_IDLE
  , P_WRITING_DIRTY_LINE
  , P_BLOCKED     // used by dummy calls from within the wrapper
} PPROC;


//forward declaration
class IScratch_mem;

/* It will become a global, configurable parameter? See armproc.cpp */
extern int LOCK_ICACHE;

class CArmProc
{
  friend class snoopdev;

  // constructors and destructor
 public:
  CArmProc();
  CArmProc(const char *armname, Mem_class *builderscra, Mem_class *builderqueue, unsigned id);
  ~CArmProc();

  // Public methods
 public:
  void Cycle(PINOUT* pinout, int* extint);
  void Reset();
  
  inline uint64_t GetRealCycles() { return m_nCycles; }
  inline uint64_t GetLogicalCycles() { return m_pCore->GetCycles();}
  inline uint64_t GetCacheHits() { return m_nCacheHits; }
  inline uint64_t GetCacheMisses() { return m_nCacheMisses; }

  inline void RegisterSWI(uint32_t nSwi, SWI_CALL* pSwi) { m_pCore->RegisterSWI(nSwi, pSwi); }
  inline void UnregisterSWI(uint32_t nSwi) { m_pCore->UnregisterSWI(nSwi); }

  void SwitchToIdleState() {m_mode = P_IDLE;};

  void RegisterCoProcessor(uint32_t nID, CCoProcessor* pCoPro);
  void UnregisterCoProcessor(uint32_t nID);

  void DebugDump();
  void DebugDumpCore();
  void DebugDumpCoProc();
  long NextPC();

 private:
  void AtomicCycle(PINOUT* pinout,int* extint);

  // Member variables
 public:
  unsigned ID;           // we need this info for a swi call
#ifndef NOSNOOP
  int CL1, CL2, CLA;  // To handle synchronization with the snoop device
#endif

 private:
  uint32_t prev_addr;
  uint32_t last_tagRAM;
  uint32_t* last_dataRAM;
  int*      intcycle;
  int*      intatomic;
  uint8_t   dummy;

  CArmCore* m_pCore;
  CCache*   m_pICache;
  CCache*   m_pDCache;
  COREBUS*  m_pCoreBus;
  COPROBUS* m_pCoProBus;

  COSTimer* m_pOSTimer;
  CIntCtrl* m_pIntCtrl;
  CLCDCtrl* m_pLCDCtrl;
  CUARTCtrl* m_pUART0Ctrl;
  //Martino
  CUARTCtrl* m_pUART1Ctrl;

  OSTBUS     m_ostbus;
  INTCTRLBUS m_icbus;
  LCDCTRLBUS m_lcdctrlbus;
  UARTCTRLBUS m_uart0ctrlbus;
  //Martino  
  UARTCTRLBUS m_uart1ctrlbus;

  // Used for storing between cycles
  uint32_t   m_addrPrev;
  bool       m_diPrev;
  PPROC      m_mode;
  uint32_t   m_nRead;

  uint64_t   m_nCycles;
  uint32_t   m_cacheLine[CACHE_LINE];
  uint32_t   m_nWrite;
  uint32_t   m_dirtyAddr;
  uint64_t   m_nCacheHits;
  uint64_t   m_nCacheMisses;

  uint32_t   m_pending;  // Did we have an interrupt whilst reading a cache line?

  CCoProcessor* m_pCoProList[16];

  const char *name;      // to give a name to this armproc

  // Required if we want to use a scratchpad
  Mem_class* scra;
  bool scratch_write;
  uint32_t scratchprev_bw;
  
  // Required if we want to use a queuedev
  Mem_class* queue;
  bool queue_write;
  uint32_t queueprev_bw;
  
  // Required if we want to use an instruction scratchpad
  IScratch_mem* iscratch;
  bool iscratch_write;
  uint32_t iscratchprev_bw;
};

#endif // __ARMPROC_H__
