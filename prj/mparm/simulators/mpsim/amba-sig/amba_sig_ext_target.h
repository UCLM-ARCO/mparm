///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_sig_target.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA bus memory slave
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_SIG_TARGET_H__
#define __AMBA_SIG_TARGET_H__

#include <systemc.h>
#include "globals.h"
#include "ext_mem.h"
#include "address.h"
#include "amba_sig_signal.h"
#include "amba_sig_target.h"
#include "power.h"
#include "stats.h"

class AMBA_SIG_EXT_MEMORY_TARGET: public amba_sig_slave
{   
 public:
  
  AMBA_SIG_EXT_MEMORY_TARGET(sc_module_name nm, uint8_t id, uint32_t start, uint32_t siz, 
                              uint mem_in_ws1) : 
    amba_sig_slave(nm,id,start,siz,mem_in_ws1)
  {
   type = "EXT_MEMORY_TARGET";
   target_mem = new Ext_mem(id, TARGET_MEM_SIZE);
   
   TRACEX(WHICH_TRACEX, 7,"%s:%d start:%x size:%x wait:%u\n",
       type, ID, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws);
   }    
};

class AMBA_SIG_EXT_SHARED_TARGET : public amba_sig_slave
{   
 public:
  
  AMBA_SIG_EXT_SHARED_TARGET(sc_module_name nm, uint8_t id, uint32_t start, uint32_t siz, 
                              uint mem_in_ws1) : 
    amba_sig_slave(nm,id,start,siz,mem_in_ws1)
  {
   type = "EXT_SHARED_TARGET";
   target_mem = new Shared_mem(id, TARGET_MEM_SIZE);   
   
   TRACEX(WHICH_TRACEX, 7,"%s:%d start:%x size:%x wait:%u\n",
       type, ID, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws);
  }
};

class AMBA_SIG_EXT_SEMAPHORE_TARGET : public amba_sig_slave
{   
 public:
  
  AMBA_SIG_EXT_SEMAPHORE_TARGET(sc_module_name nm, uint8_t id, uint32_t start, uint32_t siz, 
                    uint mem_in_ws1) : 
    amba_sig_slave(nm,id,start,siz,mem_in_ws1)
  {
   type = "EXT_SEMAPHORE_TARGET";
   target_mem = new Semaphore_mem(id, TARGET_MEM_SIZE);
   WHICH_TRACEX=SEMAPHORE_TRACEX;   
   
   TRACEX(WHICH_TRACEX, 7,"%s:%d start:%x size:%x wait:%u\n",
       type, ID, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws);
  }
};



class AMBA_SIG_EXT_INTERRUPT_TARGET : public amba_sig_slave
{ 
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
  
  // Let's raise the interrupt signal during the data phase (to be compliant with the OCCN model)
  extint[intno].write (true); //AST-OCCN
  #ifdef OCCN_DEBUG                         
        cout << "Time for interrupt: " << sc_time_stamp() << endl;
  #endif
  #ifdef PRINTDEBUG
        printf("Interrupting processor on wire %d\n", intno);
  #endif
 }

 inline uint32_t Read(uint32_t addr)
 {
  printf("Fatal error: Interrupt slave is a write only slave, received read request at time %10.1f\n",
    sc_simulation_time());
  exit(1);
  return 0; // dummy return, just to prevent compiler warnings
 }
 
 public:
 
 sc_inout<bool> *extint;

 AMBA_SIG_EXT_INTERRUPT_TARGET(sc_module_name nm, uint8_t id, uint32_t start, uint32_t siz, 
                                uint mem_in_ws1) : 
    amba_sig_slave(nm,id,start,siz,mem_in_ws1)
  {
   extint = new sc_inout<bool> [N_CORES];
  
   type = "Interrupt Slave";
      
   TRACEX(WHICH_TRACEX, 7,"%s:%d start:%x size:%x wait:%u\n",
       type, ID, START_ADDRESS,TARGET_MEM_SIZE,mem_in_ws); 
   //printf("%s %hu  - Size: 0x%08x, Base Address: 0x%08x\n\n",type, ID, TARGET_MEM_SIZE,
   //START_ADDRESS ); 
  }
};
#endif //__AMBA_SIG_TARGET_H__
