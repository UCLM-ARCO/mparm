///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         dual_clock_pinout_slave_if.h
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements the slave of a dual-clock sync module, with pinout interface
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DUAL_CLOCK_PINOUT_SLAVE_IF_H__
#define __DUAL_CLOCK_PINOUT_SLAVE_IF_H__

#include <systemc.h>
#include "core_signal.h"
#include "debug.h"

SC_MODULE(dual_clock_pinout_slave_if)
{
  public:
    sc_in<bool>        clock;
    sc_in<bool>        reset;
  
    // To master
    sc_in<bool>        request_from_wrapper;
    sc_out<bool>       ready_to_wrapper;
    sc_inout<PINOUT>   pinout;

    // To write FIFO
    sc_out<bool>       request_to_push;
    sc_out<PINOUT>     pinout_to_wrf;
    sc_in<bool>        push_full;
    sc_in<bool>        error_from_push;
    sc_in<bool>        write_overrun;
    
    // To read FIFO
    sc_out<bool>       request_to_pop;
    sc_in<PINOUT>      pinout_from_rdf;
    sc_in<bool>        pop_empty;
    sc_in<bool>        error_from_pop;
    sc_in<bool>        read_overrun;
  
    void working();
  
    SC_CTOR(dual_clock_pinout_slave_if)
    {
      SC_CTHREAD(working, clock.pos());
    }
};

#endif // __DUAL_CLOCK_PINOUT_SLAVE_IF_H__
