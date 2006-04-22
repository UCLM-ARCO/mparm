///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_traffic_gen.h
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
// info         AMBA AHB traffic generator
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_TRAFFIC_GEN_H__
#define __AMBA_AHB_TRAFFIC_GEN_H__

#include <systemc.h>
#include "amba_ahb_common.h"
#include "amba_ahb_interconnect_if.h"
#include "core_signal.h"
#include "transaction_gen.h"

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_traffic_gen - This class encapsulates an AMBA AHB traffic generator.
//                        nm: traffic generator name
//                        ID: traffic generator ID
SC_MODULE(amba_ahb_traffic_gen)
{
    sc_in<bool>                  clock;
    sc_in<bool>                  reset;
  
    sc_port<ahb_interconnect_if> master_port;
  
  private:
    // System ID
    unsigned char global_ID;
    // Local ID (on this bus alone)
    unsigned char local_ID;
    unsigned int buffer[16];
    
    transaction_gen *transgen;
    
  public:
    void working();
    
    SC_HAS_PROCESS(amba_ahb_traffic_gen);
  
    amba_ahb_traffic_gen(sc_module_name nm, unsigned char ID) : sc_module(nm), global_ID(ID)
    {
      SC_CTHREAD(working, clock.pos());
      
      transgen = new transaction_gen(ID, TGEN_TABLE);
    }
};

#endif // __AMBA_AHB_TRAFFIC_GEN_H__
