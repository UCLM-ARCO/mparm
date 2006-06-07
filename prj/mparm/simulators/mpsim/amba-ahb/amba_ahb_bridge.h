///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_bridge.h
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements a unidirectional bridge between two AMBA AHB layers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_BRIDGE_H__
#define __AMBA_AHB_BRIDGE_H__

#include <systemc.h>
#include "amba_ahb_bridge_mif.h"
#include "amba_ahb_bridge_sif.h"
#include "dual_clock_fifo.h"

SC_MODULE(amba_ahb_bridge)
{
  public:
    sc_in<bool> clk_slave;
    sc_in<bool> clk_master;
    sc_port<ahb_interconnect_if> master_port;

  private:
    sc_signal<PINOUT> *pinout_bridge1;
    sc_signal<PINOUT> *pinout_bridge2;
    sc_signal<PINOUT> *pinout_bridge3;
    sc_signal<PINOUT> *pinout_bridge4;
    
    sc_signal<bool> *RequestToPop_Mif;
    sc_signal<bool> *PopEmpty_Mif;
    sc_signal<bool> *ErrorPop_Mif;
    sc_signal<bool> *ORPop_Mif;
    
    sc_signal<bool> *RequestToPush_Mif;
    sc_signal<bool> *PushFull_Mif;
    sc_signal<bool> *ErrorPush_Mif;
    sc_signal<bool> *ORPush_Mif;
    
    sc_signal<bool> *RequestToPush_Sif;
    sc_signal<bool> *PushFull_Sif;
    sc_signal<bool> *ErrorPush_Sif;
    sc_signal<bool> *ORPush_Sif;
    
    sc_signal<bool> *RequestToPop_Sif;
    sc_signal<bool> *PopEmpty_Sif;
    sc_signal<bool> *ErrorPop_Sif;
    sc_signal<bool> *ORPop_Sif;

  public:
    amba_ahb_bridge_sif *bsif;
  private:
    amba_ahb_bridge_mif *bmif;
    fifo_controller *wr_f;
    fifo_controller *rd_f;
    
    uint bridge_ID;

  public:
    amba_ahb_bridge(sc_module_name nm, uint id);
};

#endif // __AMBA_AHB_BRIDGE_H__
