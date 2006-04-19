///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         ocp_simple_slaves.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         OCP simple slaves (memories, semaphores, interrupts)
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OCP_SIMPLE_SLAVES_H__
#define __OCP_SIMPLE_SLAVES_H__

#include <systemc.h>
#include "globals.h"
#include "ext_mem.h"
#include "address.h"
#include "ocp_slave.h"


///////////////////////////////////////////////////////////////////////////////
// ocp_private_memory - This class inherits an OCP slave and provides
//                      the functionality of a private memory.
class ocp_private_memory : public ocp_slave
{
  public:
    ocp_private_memory (sc_module_name nm,
                        uint8_t ID,
                        uint32_t start,
                        uint32_t size, 
                        uint mem_in_ws, 
                        uint mem_bb_ws)
                     : ocp_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws)
    {
      type = "ocp_private_memory";
      target_mem = new Ext_mem(ID, TARGET_MEM_SIZE);
      WHICH_TRACEX = TARGET_TRACEX;
      
      TRACEX(WHICH_TRACEX, 7, "%s %d: Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws);
    }
};

///////////////////////////////////////////////////////////////////////////////
// ocp_shared_memory - This class inherits an OCP slave and provides
//                     the functionality of a shared memory.
class ocp_shared_memory : public ocp_slave
{
  public:
    ocp_shared_memory (sc_module_name nm,
                       uint8_t ID,
                       uint32_t start,
                       uint32_t size, 
                       uint mem_in_ws, 
                       uint mem_bb_ws)
                    : ocp_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws)
    {
      type = "ocp_shared_memory";
      target_mem = new Shared_mem(ID, TARGET_MEM_SIZE);
      WHICH_TRACEX = SHARED_TRACEX;
   
      TRACEX(WHICH_TRACEX, 7, "%s %d: Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws);
    }
};

///////////////////////////////////////////////////////////////////////////////
// ocp_semaphore_device - This class inherits an OCP slave and
//                        provides the functionality of a semaphore device.
class ocp_semaphore_device : public ocp_slave
{
  public:
    ocp_semaphore_device (sc_module_name nm,
                          uint8_t ID,
                          uint32_t start,
                          uint32_t size, 
                          uint mem_in_ws, 
                          uint mem_bb_ws)
                       : ocp_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws)
    {
      type = "ocp_semaphore_device";
      target_mem = new Semaphore_mem(ID, TARGET_MEM_SIZE);
      WHICH_TRACEX = SEMAPHORE_TRACEX;
   
      TRACEX(WHICH_TRACEX, 7, "%s %d: Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws);
    }
};

///////////////////////////////////////////////////////////////////////////////
// ocp_interrupt_device - This class inherits an OCP slave and
//                             provides the functionality of an interrupt
//                             device.
//                             For this class, Read and Write methods must be
//                             overridden. Read() returns an error, since the
//                             device is write-only. Write() has the effect
//                             of raising one of the extint interrupt wires,
//                             after proper address decoding.
class ocp_interrupt_device : public ocp_slave
{
  public:
    sc_inout<bool> *extint;

  private:
    int intno;
    
    inline void Write(uint32_t addr, uint32_t data, uint8_t bw)
    {
      // Calculate the interrupt number
      intno = ((addr - START_ADDRESS) / 4) - 1;
      if (intno < 0 || intno >= N_CORES)
      {
        printf("%s: Fatal Error: cannot send interrupt on wire %d!\n", name(), intno);
        exit(1);
      }

      wait();
      extint[intno].write (true);
      
#ifdef PRINTDEBUG
      printf("Interrupting processor on wire %d @ %7.1f\n", intno, sc_simulation_time());
#endif
    }

    inline uint32_t Read(uint32_t addr)
    {
      printf("Fatal error: Interrupt slave is write only, received read request at %7.1f ns\n", sc_simulation_time());
      exit(1);
    }

  public:
    ocp_interrupt_device (sc_module_name nm,
                          uint8_t ID,
                          uint32_t start,
                          uint32_t size, 
                          uint mem_in_ws, 
                          uint mem_bb_ws)
                       : ocp_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws)
    {
      extint = new sc_inout<bool> [N_CORES];
      type = "ocp_interrupt_device";
      WHICH_TRACEX = INTERRUPT_TRACEX;

      TRACEX(WHICH_TRACEX, 7, "%s %d: Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws);
    }
};




#endif // __OCP_SIMPLE_SLAVES_H__
