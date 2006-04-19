///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_simple_slaves.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA AHB simple slaves (memories, semaphores, interrupts)
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_SIMPLE_SLAVES_H__
#define __AMBA_AHB_SIMPLE_SLAVES_H__

#include <systemc.h>
#include "globals.h"
#include "ext_mem.h"
#include "address.h"
#include "amba_ahb_slave.h"

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_private_memory - This class inherits an AMBA AHB slave and provides
//                           the functionality of a private memory.
class amba_ahb_private_memory : public amba_ahb_slave
{
  public:
    amba_ahb_private_memory (sc_module_name nm,
                             uint8_t ID,
                             uint32_t start,
                             uint32_t size, 
                             uint mem_in_ws, 
                             uint mem_bb_ws)
                          : amba_ahb_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws)
    {
      type = "amba_ahb_private_memory";
#ifdef LXBUILD
      if (CURRENT_ISS == LX)
      {
        target_mem = new Lx_mem(ID, TARGET_MEM_SIZE);
        addresser->pMem_classDebug[ID] = target_mem;
      }
#endif
#ifdef SWARMBUILD
      if(CURRENT_ISS == SWARM)
      {
        target_mem = new Ext_mem(ID, TARGET_MEM_SIZE);
      }
#endif
      WHICH_TRACEX = TARGET_TRACEX;
      
      TRACEX(WHICH_TRACEX, 7, "%s %d: Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws);
    }
};

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_shared_memory - This class inherits an AMBA AHB slave and provides
//                          the functionality of a shared memory.
class amba_ahb_shared_memory : public amba_ahb_slave
{
  public:
    amba_ahb_shared_memory (sc_module_name nm,
                            uint8_t ID,
                            uint32_t start,
                            uint32_t size, 
                            uint mem_in_ws, 
                            uint mem_bb_ws)
                         : amba_ahb_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws)
    {
      type = "amba_ahb_shared_memory";
      target_mem = new Shared_mem(ID, TARGET_MEM_SIZE);
      WHICH_TRACEX = SHARED_TRACEX;
   
      TRACEX(WHICH_TRACEX, 7, "%s %d: Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws);
    }
};

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_semaphore_device - This class inherits an AMBA AHB slave and
//                             provides the functionality of a semaphore device.
class amba_ahb_semaphore_device : public amba_ahb_slave
{
  public:
    amba_ahb_semaphore_device (sc_module_name nm,
                               uint8_t ID,
                               uint32_t start,
                               uint32_t size, 
                               uint mem_in_ws, 
                               uint mem_bb_ws)
                            : amba_ahb_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws)
    {
      type = "amba_ahb_semaphore_device";
      target_mem = new Semaphore_mem(ID, TARGET_MEM_SIZE);
      WHICH_TRACEX = SEMAPHORE_TRACEX;
   
      TRACEX(WHICH_TRACEX, 7, "%s %d: Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws);
    }
};

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_interrupt_device - This class inherits an AMBA AHB slave and
//                             provides the functionality of an interrupt
//                             device.
//                             For this class, Read and Write methods must be
//                             overridden. Read() returns an error, since the
//                             device is write-only. Write() has the effect
//                             of raising one of the extint interrupt wires,
//                             after proper address decoding.
class amba_ahb_interrupt_device : public amba_ahb_slave
{
  public:
    sc_inout<bool> *extint;

  private:
    int intno;
    
    inline void Write(uint32_t addr, uint32_t data, uint8_t bw, bool *must_wait)
    {
      // Calculate the interrupt number
      intno = ((addr - START_ADDRESS) / 4) - 1;
      
      if (intno < 0 || intno >= N_CORES)
      {
        printf("%s: Fatal Error: cannot send interrupt on wire %d!\n", name(), intno);
        exit(1);
      }

      if (CURRENT_ISS == LX)
      {
        if (data > 0)
          extint[intno].write(true);
        else
          extint[intno].write(false);
      }
      else
        extint[intno].write(true);
      
      *must_wait = false;
#ifdef OCCN_DEBUG                         
      cout << "Time for interrupt: " << sc_time_stamp() << endl;
#endif
#ifdef PRINTDEBUG
      printf("Interrupting processor on wire %d\n", intno);
#endif
    }

    inline uint32_t Read(uint32_t addr, bool *must_wait)
    {
      printf("Fatal error: Interrupt slave is write only, received read request at %7.1f ns\n", sc_simulation_time());
      exit(1);
      return 0; // Dummy return to avoid warnings - control should never get here
    }

  public:
    amba_ahb_interrupt_device (sc_module_name nm,
                               uint8_t ID,
                               uint32_t start,
                               uint32_t size, 
                               uint mem_in_ws, 
                               uint mem_bb_ws)
                            : amba_ahb_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws)
    {
      extint = new sc_inout<bool> [N_CORES];
      type = "amba_ahb_interrupt_device";
      WHICH_TRACEX = INTERRUPT_TRACEX;

      TRACEX(WHICH_TRACEX, 7, "%s %d: Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws);
    }
};

#endif // __AMBA_AHB_SIMPLE_SLAVES_H__
