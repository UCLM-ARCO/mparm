///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_occn_master.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA (OCCN) bus master
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_OCCN_MASTER_H__
#define __AMBA_OCCN_MASTER_H__

#include <systemc.h>
#include "config.h"
#include "globals.h"
#include "address.h"
#include "stats.h"
#include "occn.h"
#include "arm_bus.h"

SC_MODULE(amba_occn_master)
{
  sc_in_clk clock;
  
  sc_in<bool> request_from_wrapper;
  sc_out<bool> ready_to_wrapper;
  sc_inout<PINOUT> pinout;

  MasterPort<Pdu<AhbMasterCtrl,N_int32>,Pdu<AhbSlaveCtrl,N_int32> > port;
  uint16_t my;

  void working();
  void pulse_ready_generator();
   
  SC_CTOR(amba_occn_master)
  {
    sscanf(name(), "mst%hu", &my);

    SC_THREAD(working);
    SC_THREAD(pulse_ready_generator);
  }
  
  sc_event rdy_ev;
};

#endif // __AMBA_OCCN_MASTER_H__
