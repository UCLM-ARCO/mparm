///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         delayer_hwdata.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Signal delayer for an AMBA bus
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DELAYER_HWDATA_H__
#define __DELAYER_HWDATA_H__

#include <systemc.h>

SC_MODULE(delayer_hwdata)
{
  sc_in_clk clock;
  sc_in<bool> hready;
  sc_in<sc_uint<32> > hwdataout;
  sc_out<sc_uint<32> > delayed_hwdataout;

  void delay();

  SC_CTOR(delayer_hwdata)
  {
    SC_CTHREAD(delay, clock.pos());
  }
};

#endif // __DELAYER_HWDATA_H__
