///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_bridge_mif.h
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements the master side of an AMBA AHB bridge.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_BRIDGE_MIF_H__
#define __AMBA_AHB_BRIDGE_MIF_H__

#include <systemc.h>
#include "amba_ahb_common.h"
#include "amba_ahb_interconnect_if.h"
#include "core_signal.h"

SC_MODULE(amba_ahb_bridge_mif)
{
  public:
    sc_in<bool> clock;
  
    sc_out<bool> request_to_push;
    sc_in<bool> push_full;
    sc_in<bool> error_from_push;
    sc_in<bool> write_overrun;
    
    sc_out<bool> request_to_pop;
    sc_in<bool> pop_empty;
    sc_in<bool> error_from_pop;
    sc_in<bool> read_overrun;
  
    sc_in<PINOUT> pinout_from_wrf;
    sc_out<PINOUT> pinout_to_rdf;
  
    sc_port<ahb_interconnect_if> master_port;
  
  private:
    unsigned char ID;
    int bridge_ID;
    
    typedef enum
    {
      WAITING = 0,
      QUERY   = 1,
      READ    = 2,
      WRITE   = 3
    } MASTER_STATE;
    
  public:
    void working();
  
    SC_HAS_PROCESS(amba_ahb_bridge_mif);
  
    amba_ahb_bridge_mif(sc_module_name nm, uint bridge_ID)
      : sc_module(nm), bridge_ID(bridge_ID)
    {
      SC_CTHREAD(working, clock.pos());
    }
};

#endif // __AMBA_AHB_BRIDGE_MIF_H__
