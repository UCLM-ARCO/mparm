///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_occn_slaveint.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA (OCCN) interrupt slave
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_OCCN_SLAVEINT_H__
#define __AMBA_OCCN_SLAVEINT_H__

#include <systemc.h>
#include "config.h"
#include "globals.h"
#include "address.h"
#include "occn.h"
#include "arm_bus.h"

SC_MODULE(amba_occn_slaveint)
{
  sc_in_clk clock;
  SlavePort<Pdu<AhbSlaveCtrl,N_int32>,Pdu<AhbMasterCtrl,N_int32> > port;
  sc_inout<bool> *extint;
  
  uint16_t my;

  void receive();

   SC_HAS_PROCESS(amba_occn_slaveint);
   
   amba_occn_slaveint::amba_occn_slaveint(sc_module_name nm,N_uint sh)
       :sc_module(nm),
       shift(sh)
  {
    extint = new sc_inout<bool> [N_CORES];
    
    sscanf(name(), "s%hu", &my);
    SC_THREAD(receive);
  }

  private:

  enum operation {HREADY=1,HREWR,WRITE,WRITEHO,READ,READWRITE} op;
  N_uint shift;
  
};

#endif // __AMBA_OCCN_SLAVEINT_H__
