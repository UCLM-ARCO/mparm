///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         delayer_hsel.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Signal delayer for an AMBA bus
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DELAYER_HSEL_H__
#define __DELAYER_HSEL_H__

#include <systemc.h>
#include "globals.h"

SC_MODULE(delayer_hsel)
{
  sc_in_clk clock;
  sc_in<bool> hready;
  sc_in<bool> *hsel;
  sc_out<bool> *delayed_hsel;

  void delay();

  SC_CTOR(delayer_hsel)
  {
    hsel = new sc_in<bool> [N_SLAVES];
    delayed_hsel = new sc_out<bool> [N_SLAVES];
    SC_CTHREAD(delay, clock.pos());
  }
};

#endif // __DELAYER_HSEL_H__
