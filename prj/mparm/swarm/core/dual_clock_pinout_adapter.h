///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         dual_clock_pinout_adapter.h
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements a dual-clock sync module, with pinout interface
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DUAL_CLOCK_PINOUT_ADAPTER_H__
#define __DUAL_CLOCK_PINOUT_ADAPTER_H__

#include <systemc.h>
#include "dual_clock_pinout_slave_if.h"
#include "dual_clock_pinout_master_if.h"
#include "dual_clock_fifo.h"

SC_MODULE(dual_clock_pinout_adapter)
{
  public:
    sc_in<bool>      clk_s;
    sc_in<bool>      rst_s;
    sc_in<bool>      request_from_wrapper;
    sc_out<bool>     ready_to_wrapper;
    sc_inout<PINOUT> pinout_s;
    
    sc_in<bool>      clk_m;
    sc_in<bool>      rst_m;
    sc_out<bool>     request_to_master;
    sc_in<bool>      ready_from_master;
    sc_inout<PINOUT> pinout_m;
    
  private:
    sc_signal<PINOUT> *pinout_1;
    sc_signal<PINOUT> *pinout_2;
    sc_signal<PINOUT> *pinout_3;
    sc_signal<PINOUT> *pinout_4;

    sc_signal<bool> *RequestToPop_Sif;
    sc_signal<bool> *PopEmpty_Sif;
    sc_signal<bool> *ErrorPop_Sif;
    sc_signal<bool> *ORPop_Sif;

    sc_signal<bool> *RequestToPush_Sif;
    sc_signal<bool> *PushFull_Sif;
    sc_signal<bool> *ErrorPush_Sif;
    sc_signal<bool> *ORPush_Sif;

    sc_signal<bool> *RequestToPush_Mif;
    sc_signal<bool> *PushFull_Mif;
    sc_signal<bool> *ErrorPush_Mif;
    sc_signal<bool> *ORPush_Mif;

    sc_signal<bool> *RequestToPop_Mif;
    sc_signal<bool> *PopEmpty_Mif;
    sc_signal<bool> *ErrorPop_Mif;
    sc_signal<bool> *ORPop_Mif;

  public:
    dual_clock_pinout_slave_if *sif;
    dual_clock_pinout_master_if *mif;
    fifo_controller *wr_f;
    fifo_controller *rd_f;

    dual_clock_pinout_adapter(sc_module_name nm);
};

#endif // __DUAL_CLOCK_PINOUT_ADAPTER_H__
