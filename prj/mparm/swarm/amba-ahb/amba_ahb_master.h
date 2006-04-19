///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_master.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA AHB bus master
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_MASTER_H__
#define __AMBA_AHB_MASTER_H__

#include <systemc.h>
#include "amba_ahb_common.h"
#include "amba_ahb_interconnect_if.h"
#include "core_signal.h"

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_master - This class encapsulates an AMBA AHB master.
//                   nm: master name
//                   ID: master ID
SC_MODULE(amba_ahb_master)
{
    sc_in<bool>                  clock;
    sc_in<bool>                  reset;
  
    sc_in<bool>                  request_from_wrapper;
    sc_out<bool>                 ready_to_wrapper;
    sc_inout<PINOUT>             pinout;
  
    sc_port<ahb_interconnect_if> master_port;
  
  private:
    // System ID
    unsigned char global_ID;
    // Local ID (on this bus alone)
    unsigned char local_ID;
    
  public:
    void working();
  
    SC_HAS_PROCESS(amba_ahb_master);
  
    amba_ahb_master(sc_module_name nm, unsigned char ID) : sc_module(nm), global_ID(ID)
    {
      SC_CTHREAD(working, clock.pos());
    }
};

#endif // __AMBA_AHB_MASTER_H__
