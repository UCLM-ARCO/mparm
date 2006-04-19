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
// name   armproc.cpp
// author Michael Dales (michael@dcs.gla.ac.uk)
// header armproc.h
// info   An ARM processor - glues together the cache and what have you.
//        Includes a system coprocessor in slot 15 by default.
//
///////////////////////////////////////////////////////////////////////////////

#include "swarm.h"
#include "armproc.h"
#include <string.h>
#include "cache.h"
#include "direct.h"
#include "associative.h"
#include "setassoc.h"
#include "copro.h"
#include "syscopro.h"
#include "stats.h"
#include "scratch_mem.h"

#ifndef NO_SYS_COPRO
#define MAX_COPRO_ID 14
#else
#define MAX_COPRO_ID 15
#endif

//#define BUS_SPEED 0



/*************************************************/
/*   Macros to interact with the snoop devices   */
/*************************************************/
#ifndef NOSNOOP
#define SNOOP_LOCK()                                                         \
                       if (SNOOPING) {                                       \
                         CLA=0;                                              \
                         CL2=1;                        /* Lock the snoop */  \
                         while(CL1) {CL2=0; wait();}   /* Already locked? */ \
                         CLA=1;                                              \
                       }
#define SNOOP_UNLOCK() if (SNOOPING) CL2=0;
#else
#define SNOOP_LOCK()
#define SNOOP_UNLOCK()
#endif

int LOCK_ICACHE=0;  /* It will become a global, configurable parameter? */


///////////////////////////////////////////////////////////////////////////////
// CArmProc - Constructor
//
CArmProc::CArmProc(const char *armname, Mem_class *builderscra, Mem_class *builderqueue, unsigned id)
{
  name=armname;
  
  intcycle = (int*) calloc(NUMBER_OF_EXT,sizeof(int));
  
  intatomic = (int*) calloc(NUMBER_OF_EXT,sizeof(int));
  
  ID=id;
#ifndef NOSNOOP
  CL1=0; CL2=0; CLA=1;
#endif
  
  m_pCore = new CArmCore(this);
  m_pCoreBus = new COREBUS;
  memset(m_pCoreBus, 0, sizeof(COREBUS));
  m_pCoProBus = new COPROBUS;
  memset(m_pCoProBus, 0, sizeof(COPROBUS));
  m_addrPrev = 0;
  m_diPrev = false;  // Instruction (see core.h)
  m_pending = 0;

  if (SHARED_CACHE)
    {
      switch (UCACHETYPE)
	{
	case SETASSOC: m_pICache = new CSetAssociativeCache(UCACHESIZE, UCACHEWAYS);
	  break;
	case DIRECT:   m_pICache = new CDirectCache(UCACHESIZE);
	  break;
	case FASSOC:   m_pICache = new CAssociativeCache(UCACHESIZE);
	  break;
	default:       printf("Fatal Error: Processor %hu could not understand Unified Cache instantiation!\n", ID);
	  exit(1);
	}
      m_pICache->ID = ID; m_pICache->ID2 = 0;
      m_pDCache = m_pICache;
    }
  else
    {
      switch (ICACHETYPE)
	{
	case SETASSOC: m_pICache = new CSetAssociativeCache(ICACHESIZE, ICACHEWAYS);
	  break;
	case DIRECT:   m_pICache = new CDirectCache(ICACHESIZE);
	  break;
	case FASSOC:   m_pICache = new CAssociativeCache(ICACHESIZE);
	  break;
	default:       printf("Fatal Error: Processor %hu could not understand Instruction Cache instantiation!\n", ID);
	  exit(1);
	}
      switch (DCACHETYPE)
	{
	case SETASSOC: m_pDCache = new CSetAssociativeCache(DCACHESIZE, DCACHEWAYS);
	  break;
	case DIRECT:   m_pDCache = new CDirectCache(DCACHESIZE);
	  break;
	case FASSOC:   m_pDCache = new CAssociativeCache(DCACHESIZE);
	  break;
	default:       printf("Fatal Error: Processor %hu could not understand Data Cache instantiation!\n", ID);
	  exit(1);
	}
      m_pICache->ID = m_pDCache->ID = ID;
      m_pICache->ID2 = 0; m_pDCache->ID2 = 1;
    }

  // Scratchpad initialization (if we have one)
  scratch_write=false;
  scra=builderscra;
  scratchprev_bw=0;
  
  // Queue device initialization (if we have coreslave)
  queue_write=false;
  queue=builderqueue;
  queueprev_bw=0;
  
  // Instruction scratchpad initialization (if we have one)
  if (ISCRATCH)
    iscratch = new IScratch_mem("IScratchpad", id, addresser->ReturnIScratchSize(), addresser->ReturnIScratchPhysicalAddress(id));
  iscratch_write=false;
  iscratchprev_bw=0;
  
  prev_addr=0xFFFFFFFF;
  last_dataRAM=0;
  last_tagRAM=0;

  memset(m_pCoProList, 0, sizeof(CCoProcessor*) * 16);

#ifndef NO_SYS_COPRO
  m_pCoProList[15] = new CSysCoPro(id);
#endif // NO_SYS_COPRO

  ((CSysCoPro*)m_pCoProList[15])->RegisterCaches(m_pDCache, m_pICache);

  m_pOSTimer = new COSTimer(name);
  m_pIntCtrl = new CIntCtrl(name);
  m_pLCDCtrl = new CLCDCtrl();
  m_pUARTCtrl = new CUARTCtrl(name);

  m_nCycles = 0;
  m_nCacheHits = 0;
  m_nCacheMisses = 0;
  m_mode = P_NORMAL;

  Reset();
}



///////////////////////////////////////////////////////////////////////////////
// ~CArmProc - Destructor
//
CArmProc::~CArmProc()
{
  cout << "Cache info: hits = " << m_nCacheHits << " misses = " <<
    m_nCacheMisses << "\n";

  // Clean up caches - check to see if they are the same.
  if (m_pICache == m_pDCache)
    {
      // Same - so only delete once
      delete m_pICache;
    }
  else
    {
      // Different - delete each one
      delete m_pICache;
      delete m_pDCache;
    }

  for (int i = 0; i < 16; i++)
    if (m_pCoProList[i] != NULL)
      delete m_pCoProList[i];

  delete m_pIntCtrl;
  delete m_pOSTimer;
  delete m_pLCDCtrl;
  delete m_pUARTCtrl;

  delete m_pCore;
  delete m_pCoreBus;
  delete m_pCoProBus;
}



///////////////////////////////////////////////////////////////////////////////
// Reset
//
void CArmProc::Reset()
{
  memset(&m_ostbus, 0, sizeof(OSTBUS));
  memset(&m_icbus, 0, sizeof(INTCTRLBUS));
  memset(&m_lcdctrlbus, 0, sizeof(LCDCTRLBUS));
  memset(&m_uartctrlbus, 0, sizeof(UARTCTRLBUS));
}



///////////////////////////////////////////////////////////////////////////////
// AtomicCycle - Cycles internal devices and pipeline
//
void CArmProc::AtomicCycle(PINOUT* pinout,int* extint)
{ 
  uint support=0,state,i;
  uint32_t temp = m_pCoreBus->Din;

  if(extint!=0)
    for(i=0;i<NUMBER_OF_EXT;i++)
      {if(extint[i]) 
	{intatomic[i]++;
	//printf("Atomic_cycle:%d,intatomic[%d]:%d\n",ID,i,intatomic[i]);
	}
      }
  
  // Cycle any on chip aids
  m_icbus.intbits = 0;

  m_pOSTimer->Cycle(&m_ostbus);
  if (m_ostbus.interrupt)
    m_icbus.intbits |= (m_ostbus.interrupt << 26);

  m_pLCDCtrl->Cycle(&m_lcdctrlbus);
  if (m_lcdctrlbus.interrupt)
    m_icbus.intbits |= (0x1 << 25);

  m_pUARTCtrl->Cycle(&m_uartctrlbus);
  if (m_uartctrlbus.interrupt)
    m_icbus.intbits |= (0x1 << 24);
    
  //set the intctrl bus
  for(i=0;i<NUMBER_OF_EXT;i++)
    {if(intatomic[i]!=0) 
      {
	m_icbus.intbits |= (0x1 <<  (23-i) );
	//printf("%s Received intbits:%x,%d\n",name,m_icbus.intbits,i);
      }
    }
  
  m_pIntCtrl->Cycle(&m_icbus);

  // Clear these for generation by the core next time round
  m_ostbus.r = m_ostbus.w = 0;
  m_icbus.r = m_icbus.w = 0;
  m_lcdctrlbus.r = m_lcdctrlbus.w = 0;
  m_uartctrlbus.r = m_uartctrlbus.w = 0;

  // Generate the interrupt bits
  m_pCoreBus->fiq = pinout->fiq && m_icbus.fiq;
  m_pCoreBus->irq = pinout->irq && m_icbus.irq;
  m_pCoProBus->fiq = pinout->fiq && m_icbus.fiq;
  m_pCoProBus->irq = pinout->irq && m_icbus.irq;
  
  if (m_pIntCtrl->breakpoint!=0) 
    {
      //printf("\n%s: irq at pc=%x,intctrl:%x",nome,NextPC(),m_pIntCtrl->m_regs[0]);
      state=m_pIntCtrl->breakpoint; //new interrupt
      state&=0xFFFFFF;
      //If state !=0, a new int between 0 and 23 has been asserted (external interrupt!)
      if (state)
	{ 
	  for(i=0;i<24;i++)
	    {support = state & (1 << i);									
	    if (support!=0) break;									
	    }
	  if (intatomic[23 - i]!=0) 
	    {
	      intatomic[23 - i]--;
	      //printf("Atomic_cycle:%d,intatomic[%d]:%d\n",ID,(23-i),intatomic[23-i]);
	    }
	}
      //Poletti check: is this OK?
      m_pIntCtrl->breakpoint=0;
    };

  if ((m_pCoProBus->dw == 1) && (m_pCoreBus->enout != 0))
    m_pCoreBus->Din = m_pCoProBus->Dout;

  if (m_pCoreBus->A & 0x80000000)
    {
      // Find out which internal device we're talking to
      if ((m_pCoreBus->A & 0xFFFF0000) == 0x90050000)
	{
	  // The interrupt controller
	  m_pCoreBus->Din = m_icbus.data;
	}
      else if ((m_pCoreBus->A & 0xFFFF0000) == 0x90000000)
	{
	  // The OS Timer
	  m_pCoreBus->Din = m_ostbus.data;
	}
      else if ((m_pCoreBus->A & 0xFFFFF000) == 0x90081000)
	{
	  // The UART Controller
	  m_pCoreBus->Din = m_uartctrlbus.data;
	}
      else if ((m_pCoreBus->A & 0xFFF00000) == 0x90100000)
	{
	  // The LCD Controller
	  m_pCoreBus->Din = m_uartctrlbus.data;
	}
    }

  m_pCore->Cycle(m_pCoreBus);

  m_pCoProBus->opc = m_pCoreBus->opc;
  m_pCoProBus->cpi = m_pCoreBus->cpi;
  m_pCoProBus->cpa = m_pCoreBus->cpa;
  m_pCoProBus->cpb = 1;
  m_pCoProBus->dw = 0;
  if (m_pCoreBus->rw == 1)
    if (m_pCoreBus->opc == 1)
      m_pCoProBus->Din = temp;
    else
      m_pCoProBus->Din = m_pCoreBus->Dout;
  else
    m_pCoProBus->Din = m_pCoreBus->Din;

  for (int ii = 0; ii < 16; ii++)
    if (m_pCoProList[ii] != NULL)
      m_pCoProList[ii]->Cycle(m_pCoProBus);

  m_pCoreBus->cpa = m_pCoProBus->cpa;
  m_pCoreBus->cpb = m_pCoProBus->cpb;

  if ((m_pCoProBus->dw == 1) && (m_pCoreBus->enout == 0))
    m_pCoreBus->Dout = m_pCoProBus->Dout;
}

#define PENDING_FIQ   0x1
#define PENDING_IRQ   0x2
#define PENDING_RESET 0x4




///////////////////////////////////////////////////////////////////////////////
// Cycle - Main processor execution cycle
//
void CArmProc::Cycle(PINOUT* pinout,int* extint)
{

  if (m_mode==P_IDLE)
    {
      /* Core in IDLE state */
      if (STATS)
	statobject->inspectSWARMAccess(0, m_mode, false, 0, ID);
      return;
    }

  if(extint!=0) 
    for(uint i=0;i<NUMBER_OF_EXT;i++)
      {
	if(extint[i]!=0) 
	  {
	    intcycle[i]++;
	    //printf("Armproc_cycle:%d,%d\n",i,intcycle[i]);
	  }
      }// Interrupt received, increase counter of interrupts still to be passed to AtomicCycle

  switch (m_mode)
    {
    case P_NORMAL:
      {
	uint32_t addr;

	if (m_pending & PENDING_FIQ)
	  {
	    pinout->fiq = 0;
	    m_pending &= ~PENDING_FIQ;
	  }
	if (m_pending & PENDING_IRQ)
	  {
	    pinout->irq = 0;
	    m_pending &= ~PENDING_IRQ;
	  }

	addr = m_pCoreBus->A & 0xFFFFFFFC;
        
	//if(m_pCoreBus->di)
	//printf("core %hu - want Daddress %d\n", ID, addr);
	//else
	//printf("core %hu - want Iaddress %d\n", ID, addr);

	// See if it's a memory or internal read
	if ((addr & 0x80000000) == 0x00000000)
	  {
	    if(addresser->PhysicalInIScratchSpace(ID, addresser->Logical2Physical(addr, ID)))
	      {
		m_pCoreBus->Din = iscratch->Read(addr);
		pinout->benable=0;
	      }
	    else
	      if(addresser->PhysicalInScratchSpace(ID, addresser->Logical2Physical(addr, ID), &dummy))
		{
		  //if(m_pCoreBus->rw == 0)
		  if(SCRATCH_WS) wait(SCRATCH_WS);
		  m_pCoreBus->Din = scra->Read(addr);
		  pinout->benable=0;
		}
	      else
		if(addresser->PhysicalInQueueSpace(ID, addresser->Logical2Physical(addr, ID)))
		  {
		    m_pCoreBus->Din = queue->Read(addr);
		    pinout->benable=0;
		  }
		else
		  {
		    // Accessing memory
		    // Assume we are reading a value

		    ASSERT(m_pCoreBus->rw == 0);

		    if (!addresser->LogicalIsCacheable(addr) && 
			!addresser->PhysicalInScratchSpace(ID, addresser->Logical2Physical(addr,ID),&dummy) &&
			!addresser->PhysicalInIScratchSpace(ID, addresser->Logical2Physical(addr,ID)) &&
			!addresser->PhysicalInQueueSpace(ID, addresser->Logical2Physical(addr, ID))
			)
		      {
			m_pCoreBus->Din = pinout->data;
		      }
		    else
		      {
	      
			//CCache* pCache = m_pCoreBus->di ? m_pDCache : m_pICache;
			uint32_t* dataRAM = 0;
			uint32_t  tagRAM = 0;
			uint32_t  dirtyRAM = 0;

			m_nCacheHits++;
			if(m_pCoreBus->di)
			  {
			    //printf("core %hu - Dcache read:%d\n", ID, addr);
			    SNOOP_LOCK();
			    if(DCACHE_WS) wait(DCACHE_WS);
			    dataRAM = m_pDCache->Read_dataRAM(addr >> 2);
			    tagRAM  = m_pDCache->Read_tagRAM(addr >> 2);
			    dirtyRAM  = m_pDCache->Read_dirtyRAM(addr >> 2);
			    SNOOP_UNLOCK();
			  }
			else
			  {
			    if (prev_addr!=addr)
			      {
				//printf("core %hu - Icache read:%d\n", ID, addr);
				if (SHARED_CACHE || LOCK_ICACHE) SNOOP_LOCK();
				if(ICACHE_WS) wait(ICACHE_WS);
				dataRAM = m_pICache->Read_dataRAM(addr >> 2);
				tagRAM  = m_pICache->Read_tagRAM(addr >> 2);
				dirtyRAM  = m_pICache->Read_dirtyRAM(addr >> 2);
				ASSERT (SHARED_CACHE || ((dirtyRAM&DIRTY_BIT) != DIRTY_BIT)); // instruction cannot be dirty but can be invalid
				SNOOP_UNLOCK();
			      }
			    else
			      {
				dataRAM = last_dataRAM;
				tagRAM = last_tagRAM;
				//printf("core %hu - Repeated:%d\n", ID, last_dataRAM [(addr >> 2) & 0x00000003] );
			      }
			  }

			if (
			    ( !m_pCoreBus->di && (prev_addr==addr) ) ||
			    ( (tagRAM>>2) == (addr>>4)) && ( (dirtyRAM & INVALID_BIT) != INVALID_BIT )
			    )
			  { // cache hit
		  
			    if (!m_pCoreBus->di) {
			      last_dataRAM = dataRAM;
			      last_tagRAM = tagRAM;
			      prev_addr = addr;
			    }
			    m_pCoreBus->Din = dataRAM [(addr>>2) & 0x00000003];
		  
			    //if(m_pCoreBus->di)
			    //printf("core %hu - got Ddata %d\n", ID, m_pCoreBus->Din);
			    //else
			    //printf("core %hu - got Idata %d\n", ID, m_pCoreBus->Din);
		  
			  } else { // cache miss

			    //if(m_pCoreBus->di)
			    //printf("core %hu - Dcache miss\n", ID);
			    //else
			    //printf("core %hu - Icache miss\n", ID);
		  
			    // We're not going to clock the core for a while as we
			    // read in the cache line, so we'd better be ready to note
			    // interrupts.
			    if (pinout->fiq == 0)
			      m_pending |= PENDING_FIQ;
			    if (pinout->irq == 0)
			      m_pending |= PENDING_IRQ;
		  
			    m_nCacheMisses++;
			    m_nCacheHits--;
		  
			    if (STATS)
			      statobject->inspectSWARMAccess(addr, m_mode, false, m_pCoreBus->di, ID);
		  
			    if ( (CACHE_WRITE_POLICY != WT) && ((dirtyRAM & DIRTY_BIT) == DIRTY_BIT) ) {
		    
			      //printf("core %hu - caught dirty data\n", ID);
			      // instruction cannot be DIRTY
			      ASSERT (m_pCoreBus->di);

			      m_dirtyAddr = (tagRAM << 2) & ~0x0000000F;
		    
			      for(int l=0;l < CACHE_LINE; l++) {
				m_cacheLine[l]=dataRAM[l];
			      }
		    
			      m_nWrite = 0;
			      pinout->address = m_dirtyAddr;
			      pinout->data = m_cacheLine[0];
			      //printf("core %hu - writing %d @ %d\n", ID, m_cacheLine[0], m_dirtyAddr);
			      pinout->rw = 1;
			      pinout->bw = 0;
			      pinout->benable = 1;
			      pinout->burst = 1;
		    
			      m_mode = P_WRITING_DIRTY_LINE;
		    
		    
			    } else { // data read is clean
			      m_mode = P_READING1;
			      pinout->benable=0;
			    }
		  
			    break;

			  }

		      }
		  }
	  }
	else
	  {
	    // Reading from a internal device
	    // See in Atomic cycle for the code here
	  }


	switch (m_pCoreBus->bw)
	  {
	  case 0:		// Read word
	    {
	      // Was the addess unaligned? If so do the rotate so that the
	      // index byte is in the lowest position.
	      uint32_t rot = m_pCoreBus->A & 0x00000003;
	      uint32_t ttemp = m_pCoreBus->Din >> (rot * 8);
	      m_pCoreBus->Din = ttemp | (m_pCoreBus->Din << ((4 - rot) * 8));
	    }
	    break;
	  case 1:		// Read byte
	    {
	      // Reading a byte, so mung the Din correctly
	      uint32_t nByte = m_pCoreBus->A & 0x00000003;
	      m_pCoreBus->Din = m_pCoreBus->Din >> (8 * nByte);
	      m_pCoreBus->Din &= 0x000000FF;
	      //printf("Read byte 0x%x @ 0x%x\n", m_pCoreBus->Din, m_pCoreBus->A);
	    }
	    break;
	  case 2:		// Read half word
	    {
	      // Check the alignment, if necessary rotate 16 bits
	      if ((m_pCoreBus->A & 0x00000002) == 0x00000002)
		m_pCoreBus->Din = (m_pCoreBus->Din >> 16);
	      else
		m_pCoreBus->Din &= 0x0000FFFF;
	    }
	    break;
	  }

	if (STATS)
	  statobject->inspectSWARMAccess(addr, m_mode, true, m_pCoreBus->di, ID);

	/////////////////////////////////////////////
	// *** Cycle the core and coprocessors *** // 
	//                                         //
	AtomicCycle(pinout, intcycle);
        for(uint i=0;i<NUMBER_OF_EXT;i++)
	  {
	    if(intcycle[i]) 
	      {
		intcycle[i]--;
		//printf("Armproc_cycle1:%d,%d\n",i,intcycle[i]);
	      }
	  }
	//                                         //
	/////////////////////////////////////////////

	/* Why was it needed?
	   if (m_pCoreBus->swi_hack == 1)
	   {
	   m_pICache->Reset();
	   if (m_pICache != m_pDCache)
	   m_pDCache->Reset();

	   m_pCoreBus->swi_hack = 0;
	   }
	*/

	// Find out how the next cycle is going to go
	if (m_pCoreBus->A & 0x80000000)
	  {
	    // Top bit of address set - means that we're reading/writing
	    // to stuff on the chip (e.g. interrupt controller)
	    if ((m_pCoreBus->rw == 1) && (m_pCoreBus->enout == 0))
	      {
		// We'll be writing, so make a note of this address for the
		// next cycle when we'll use it.
		m_addrPrev = m_pCoreBus->A;
		m_diPrev = m_pCoreBus->di;
		m_mode = P_INTWRITE;
	      }

	    if ((m_pCoreBus->rw == 0) && (m_pCoreBus->enout == 0))
	      {
		// Find out which internal device we're talking to
		if ((m_pCoreBus->A & 0xFFFF0000) == 0x90050000)
		  {
		    // The interrupt controller
		    m_icbus.addr = m_pCoreBus->A & 0x0000FFFF;
		    m_icbus.r = 1;
		  }
		else if ((m_pCoreBus->A & 0xFFFF0000) == 0x90000000)
		  {
		    // The OS Timer
		    m_ostbus.addr = m_pCoreBus->A & 0x0000FFFF;
		    m_ostbus.r = 1;
		  }
		else if ((m_pCoreBus->A & 0xFFFFF000) == 0x90081000)
		  {
		    // The UART Controller
		    m_uartctrlbus.addr = m_pCoreBus->A & 0x00000FFF;
		    m_uartctrlbus.r = 1;
		  }
		else if ((m_pCoreBus->A & 0xFFF00000) == 0x90100000)
		  {
		    // The LCD Controller
		    m_lcdctrlbus.addr = m_pCoreBus->A & 0x000FFFFF;
		    m_lcdctrlbus.r = 1;
		  }
	      }
	  }
	else
	  // Normal talking to real world.
	  if ((m_pCoreBus->rw == 1) && (m_pCoreBus->enout == 0))
	    {
	      // We'll be writing, so make a note of this address for the
	      // next cycle when we'll use it.
	      m_addrPrev = m_pCoreBus->A;
	      m_diPrev = m_pCoreBus->di;
	      m_mode = P_WRITING1;
	      if (addresser->PhysicalInScratchSpace(ID, addresser->Logical2Physical(addr,ID), &dummy) 
		  || addresser->PhysicalInIScratchSpace(ID, addresser->Logical2Physical(addr,ID))
		  || addresser->PhysicalInQueueSpace(ID,addresser->Logical2Physical(addr,ID)))
		pinout->benable=0;
	    }
	  else if ((m_pCoreBus->rw == 0) && (m_pCoreBus->enout == 0))
	    // We're reading. See if the address should be read from
	    // the cache or from non-cacheable areas
	    if (!addresser->LogicalIsCacheable(m_pCoreBus->A) 
		&& !addresser->PhysicalInScratchSpace(ID,addresser->Logical2Physical(m_pCoreBus->A, ID), &dummy)
		&& !addresser->PhysicalInIScratchSpace(ID,addresser->Logical2Physical(m_pCoreBus->A, ID))
		&& !addresser->PhysicalInQueueSpace(ID,addresser->Logical2Physical(m_pCoreBus->A,ID))
		)
	      m_mode = P_READING1;
	pinout->benable = 0;
      }
      break;

    case P_READING1:               // Reading in a cache line here.
      {
	if (pinout->fiq == 0)
	  m_pending |= PENDING_FIQ;
	if (pinout->irq == 0)
	  m_pending |= PENDING_IRQ;

	if(!addresser->LogicalIsCacheable(m_pCoreBus->A))
	  {
	    pinout->address = m_pCoreBus->A & 0xFFFFFFFC;
	    pinout->rw = 0;
	    pinout->bw = 0;
	    pinout->benable = 1;
	    pinout->burst = 1;

	    if (STATS)
	      statobject->inspectSWARMAccess(m_pCoreBus->A, m_mode, true, m_pCoreBus->di, ID);

	    m_mode = P_NORMAL;
	  }
	else
	  {
	    m_nRead = 0;
	    pinout->address = m_pCoreBus->A & 0xFFFFFFF0;
	    pinout->rw = 0;
	    pinout->bw = 0;
	    pinout->benable = 1;
	    pinout->burst = 4;

	    if (STATS)
	      statobject->inspectSWARMAccess(m_pCoreBus->A, m_mode, true, m_pCoreBus->di, ID);

	    m_mode = P_READING;
	    // Add extra cycle for initiating a read
	    // m_nCycles += BUS_SPEED;
	  }
      }
      break;

    case P_READING:
      {
	m_cacheLine[m_nRead] = pinout->data;
	m_nRead++;

	if (pinout->fiq == 0)
	  m_pending |= PENDING_FIQ;
	if (pinout->irq == 0)
	  m_pending |= PENDING_IRQ;

	if (STATS)
	  statobject->inspectSWARMAccess(0, m_mode, true, m_pCoreBus->di, ID);

	if (m_nRead < CACHE_LINE)
	  {
	    // Set up to read the next word in the cache line
	    pinout->address = (m_pCoreBus->A & 0xFFFFFFF0) + (m_nRead * 4);
	    pinout->rw = 0;
	    pinout->benable = 1;
	    // m_nCycles += BUS_SPEED;
	  }
	else
	  {
	    // Write the full line into the cache
	    
	    if(m_pCoreBus->di)
	      {
		//printf("core %hu - Writing line to Dcache %d -> %d %d %d %d\n",ID,((m_pCoreBus->A & 0xFFFFFFF0) >> 2),m_cacheLine[0],m_cacheLine[1],m_cacheLine[2],m_cacheLine[3]);
		SNOOP_LOCK();
		if(DCACHE_WS) wait(DCACHE_WS);
		m_pDCache->WriteLine(((m_pCoreBus->A & 0xFFFFFFF0) >> 2), m_cacheLine);
		SNOOP_UNLOCK();
	      }
	    else
	      {
		//printf("core %hu - Writing line to Icache %d -> %d %d %d %d\n",ID,((m_pCoreBus->A & 0xFFFFFFF0) >> 2),m_cacheLine[0],m_cacheLine[1],m_cacheLine[2],m_cacheLine[3]);
		if (SHARED_CACHE || LOCK_ICACHE) SNOOP_LOCK();
		if(ICACHE_WS) wait(ICACHE_WS);
		m_pICache->WriteLine(((m_pCoreBus->A & 0xFFFFFFF0) >> 2), m_cacheLine);
		SNOOP_UNLOCK();
	      }
	    
	    pinout->benable = 0;
	    m_mode = P_NORMAL;
	  }
      }
      break;

    case P_WRITING_DIRTY_LINE:
      {
        m_nWrite++;

        if (pinout->fiq == 0)
          m_pending |= PENDING_FIQ;
        if (pinout->irq == 0)
          m_pending |= PENDING_IRQ;
       
        if (STATS)
          statobject->inspectSWARMAccess(0, m_mode, true, m_pCoreBus->di, ID);
        
        if (m_nWrite < CACHE_LINE) {
          pinout->address = m_dirtyAddr + (m_nWrite * 4);
          pinout->data = m_cacheLine[m_nWrite];
	  //printf("core %hu - writing %d @ %d\n", ID, m_cacheLine[m_nWrite], m_dirtyAddr + (m_nWrite * 4));
	  pinout->rw = 1;
          pinout->benable = 1;
        } else {
          m_pDCache->UnsetBit(m_dirtyAddr >> 2, DIRTY_BIT);
          m_mode = P_NORMAL;
        }
      }
      break;

    case P_WRITING1:
      {
        bool write_through_hit = false;
	bool flag = false;

        if (m_diPrev == false)
	  {
	    printf("Fatal Error: Core %hu is trying to write an instruction!\n", ID);
	    exit(1);
	  }

	if (addresser->PhysicalInScratchSpace(ID, addresser->Logical2Physical(m_pCoreBus->A, ID), &dummy))
	  {
	    scratchprev_bw=m_pCoreBus->bw;
	    scratch_write=true;
	  }
	else
	  if (addresser->PhysicalInIScratchSpace(ID, addresser->Logical2Physical(m_pCoreBus->A, ID)))
	    {
	      iscratchprev_bw=m_pCoreBus->bw;
	      iscratch_write=true;
	    }
	  else
	    if (addresser->PhysicalInQueueSpace(ID, addresser->Logical2Physical( m_pCoreBus->A,ID)))
	      {
		queueprev_bw=m_pCoreBus->bw;
		queue_write=true;
	      }
	    else
	      pinout->bw = m_pCoreBus->bw;

	/////////////////////////////////////////////
	// *** Cycle the core and coprocessors *** // 
	//   
        //
	AtomicCycle(pinout, intcycle);
	for(uint i=0;i<NUMBER_OF_EXT;i++)
	  {
	    if(intcycle[i]) 
	      {
		intcycle[i]--;
		//printf("Armproc_cycle1:%d,%d\n",i,intcycle[i]);
	      }
	  }
	//                                         //
	/////////////////////////////////////////////

	// If this is a scratchpad write, I put the data in the scra
	if (scratch_write)
	  {
	    if(SCRATCH_WS) wait(SCRATCH_WS);
	    scra->Write(m_addrPrev, m_pCoreBus->Dout, scratchprev_bw);
	    scratch_write=false;
	    pinout->benable=0;
	  }
	else
	  if (iscratch_write)
	    {
	      iscratch->Write(m_addrPrev, m_pCoreBus->Dout, iscratchprev_bw);
	      iscratch_write=false;
	      pinout->benable=0;
	    }
	  else
	    if (queue_write)
	      {
		queue->Write(m_addrPrev, m_pCoreBus->Dout, queueprev_bw);
		queue_write=false;
		pinout->benable=0;
	      }
	    else
	      {
		// Set the bus up to do the write.
		if (CACHE_WRITE_POLICY == WT){
		  pinout->benable = 1;
		} else {
		  flag=true;
		}
		pinout->address = m_addrPrev;
		pinout->data = m_pCoreBus->Dout;
		pinout->rw = 1;
		pinout->burst = 1;

		//printf("core %hu - writing 0x%x @ 0x%x\n", ID, pinout->data, pinout->address);

		// Add extra cycle for cost of write.
		// m_nCycles += BUS_SPEED;
	      }

	// If the write is to cacheable spaces (not including scratchpad), write through the cache
	if (addresser->LogicalIsCacheable(m_addrPrev) && !addresser->PhysicalInScratchSpace(ID, addresser->Logical2Physical(m_addrPrev, ID), &dummy)) {
	  //CCache* pCache = m_diPrev ? m_pDCache : m_pICache;
	  uint32_t tagRAM;
	  uint32_t dirtyRAM;
	  CCache* pCache;
	  if(m_diPrev)
	    {
	      // Is the data in the cache?
	      SNOOP_LOCK();
	      for (int i=0; i< DCACHE_WS; i++) wait();
	      tagRAM = m_pDCache->Read_tagRAM(pinout->address >> 2);
	      dirtyRAM = m_pDCache->Read_dirtyRAM(pinout->address >> 2);
	      // Let's just check the tags. Data is returned, but just for simulation purposes
	      SNOOP_UNLOCK();
	      pCache = m_pDCache;
	    }
	  else
	    {
	      // Is the data in the cache?
	      if (SHARED_CACHE || LOCK_ICACHE) SNOOP_LOCK(); 
	      for (int i=0; i< ICACHE_WS; i++) wait();
	      tagRAM = m_pICache->Read_tagRAM(pinout->address >> 2);
	      dirtyRAM = m_pICache->Read_dirtyRAM(pinout->address >> 2);
	      // Let's just check the tags. Data is returned, but just for simulation purposes
	      SNOOP_UNLOCK();
	      pCache = m_pICache;
	    }
	 
	  if ( ((tagRAM>>2) == (pinout->address>>4)) && ((dirtyRAM & INVALID_BIT) != INVALID_BIT) ) {

	    // cache hit
	   
	    write_through_hit = true;

	    // arranging word according to bw done in cache
	    // this avoids 

	    if (SHARED_CACHE || LOCK_ICACHE || m_diPrev) SNOOP_LOCK();
	    for (int i=(m_diPrev?DCACHE_WS:ICACHE_WS); i>0 ; i--) wait();
	    pCache->WriteWord((pinout->address >> 2), pinout->data, pinout->bw, (pinout->address & 0x3) );
	    SNOOP_UNLOCK();
	   
	   

	  } else { // cache miss
	   
	    write_through_hit = false;
	    if (CACHE_WRITE_POLICY != WT)
	      pinout->benable = 1;
	  }
	 
	  if (STATS)
	    statobject->inspectSWARMAccess(m_addrPrev, m_mode, write_through_hit, m_diPrev, ID);
	 
	} //end of the write into the cache
	else {
	  if (flag==true) {
	    ASSERT (CACHE_WRITE_POLICY != WT);
	    pinout->benable = 1;
	    //printf("core %hu - writing %d @ %d\n", ID, pinout->data, pinout->address);
	   
	  }
	  if (STATS)
	    statobject->inspectSWARMAccess(m_addrPrev, m_mode, false, m_diPrev, ID);
	}
	// What are we to do next?
	if (m_pCoreBus->rw == 1)
	  {
	    // We'll be writing, so make a note of this address for the
            // next cycle when we'll use it.
	    m_addrPrev = m_pCoreBus->A;
	    m_diPrev = m_pCoreBus->di;
	    m_mode = P_WRITING1;
	  }
	else
	  m_mode = P_NORMAL;

      }
      break;
    case P_INTWRITE:
      {
	// An internal write - writing to part of the address space inside
	// the coprocessor.
	//if (extint) printf("%s Passing int\n",name);

	/////////////////////////////////////////////
	// *** Cycle the core and coprocessors *** // 
	// 
	//
	AtomicCycle(pinout, intcycle);
	for(uint i=0;i<NUMBER_OF_EXT;i++)
	  {
	    if(intcycle[i]) 
	      {
		intcycle[i]--;
		//printf("Armproc_cycle1:%d,%d\n",i,intcycle[i]);
	      }
	  }
	//                                          //
	/////////////////////////////////////////////

	// Not using real memory at any point
	pinout->benable = 0;

	// Find out which internal device we're talking to
	if ((m_addrPrev & 0xFFFF0000) == 0x90050000)
	  {
	    // The interrupt controller 
	    m_icbus.addr = m_addrPrev & 0x0000FFFF;
	    m_icbus.w = 1;
	    m_icbus.data = m_pCoreBus->Dout;
	  }
	else if ((m_addrPrev & 0xFFFF0000) == 0x90000000)
	  {
	    // The OS Timer
	    m_ostbus.addr = m_addrPrev & 0x0000FFFF;
	    m_ostbus.w = 1;
	    m_ostbus.data = m_pCoreBus->Dout;
	  }
	else if ((m_addrPrev & 0xFFFFF000) == 0x90081000)
	  {
	    // The UART Controller
	    m_uartctrlbus.addr = m_addrPrev & 0x00000FFF;
	    m_uartctrlbus.w = 1;
	    m_uartctrlbus.data = m_pCoreBus->Dout;
	  }
	else if ((m_addrPrev & 0xFFF00000) == 0x90100000)
	  {
	    // The LCD Controller
	    m_lcdctrlbus.addr = m_addrPrev & 0x000FFFFF;
	    m_lcdctrlbus.w = 1;
	    m_lcdctrlbus.data = m_pCoreBus->Dout;
	  }

	if (STATS)
	  statobject->inspectSWARMAccess(m_addrPrev, m_mode, true, m_diPrev, ID);

	m_mode = P_NORMAL;
      }
      break;

    default:
      ASSERT(0);
    }

  m_nCycles++;

}


///////////////////////////////////////////////////////////////////////////////
// RegisterCoProcessor - 
//
void CArmProc::RegisterCoProcessor(uint32_t nID, CCoProcessor* pCoPro)
{
  if ((pCoPro == NULL) || (nID > MAX_COPRO_ID))
    throw CCoProInvalidException();
  if (m_pCoProList[nID] != NULL)
    throw CCoProSetException();

  m_pCoProList[nID] = pCoPro;
}


///////////////////////////////////////////////////////////////////////////////
// UnregisterCoProcessor - 
//
void CArmProc::UnregisterCoProcessor(uint32_t nID)
{
  if (nID > MAX_COPRO_ID)
    throw CCoProInvalidException();

  m_pCoProList[nID] = NULL;
}


///////////////////////////////////////////////////////////////////////////////
// DebugDump - Dump info for core and coprocessors.
//
void CArmProc::DebugDump()
{
  m_pCore->DebugDump();

  for (int i = 0; i < 16; i++)
    if (m_pCoProList[i] != NULL)
      m_pCoProList[i]->DebugDump();
}


///////////////////////////////////////////////////////////////////////////////
// DebugDumpCore - Dump info for core and coprocessors.
//
void CArmProc::DebugDumpCore()
{
  m_pCore->DebugDump();
}

///////////////////////////////////////////////////////////////////////////////
// DebugDumpCoProc - Dump info for core and coprocessors.
//
void CArmProc::DebugDumpCoProc()
{
  for (int i = 0; i < 16; i++)
    if (m_pCoProList[i] != NULL)
      m_pCoProList[i]->DebugDump();
}

///////////////////////////////////////////////////////////////////////////////
// NextPC - Gets the next PC value
//
long CArmProc::NextPC()
{
  return m_pCore->NextPC();
}
