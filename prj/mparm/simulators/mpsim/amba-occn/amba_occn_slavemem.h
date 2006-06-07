///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_occn_slavemem.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA (OCCN) memory slave
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_OCCN_SLAVEMEM_H__
#define __AMBA_OCCN_SLAVEMEM_H__

#include <systemc.h>
#include "globals.h"
#include "ext_mem.h"
#include "address.h"
#include "occn.h"
#include "arm_bus.h"

class amba_occn_slavemem : public sc_module
{

 public:
  sc_in_clk clock;
  
  SlavePort<Pdu<AhbSlaveCtrl,N_int32>,Pdu<AhbMasterCtrl,N_int32> > port;

  void receive();
 
 private: 
  uint16_t my;
  Mem_class* myRam;

  enum operation {HREADY=1,HREWR,WRITE,WRITEHO,READ,READWRITE} op;
  N_uint shift;

  SC_HAS_PROCESS(amba_occn_slavemem);

 public:
  amba_occn_slavemem::amba_occn_slavemem(sc_module_name nm, N_uint sh)
  :sc_module(nm),
   shift(sh)
  {
    SC_THREAD(receive);

    sscanf(name(), "s%hu", &my);
    if (addresser->IsShared(my))          // Shared Memory
      myRam = new Shared_mem(my, addresser->ReturnSharedSize(my));
    else if (addresser->IsSemaphore(my))        // Semaphore Memory
      myRam = new Semaphore_mem(my, addresser->ReturnSemaphoreSize(my));
    else                                  // Private Memory
      myRam = new Ext_mem(my, addresser->ReturnPrivateSize());
  }

  
};

#endif // __AMBA_OCCN_SLAVEMEM_H__
