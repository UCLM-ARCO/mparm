///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Università di Bologna
//
// name         freq_register.h
// author       DEIS - Università di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              José L. Ayala - jayala@die.upm.es
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Davide Bertozzi - dbertozzi@deis.unibo.it
// info         Programmable register which feeds the frequency divider
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FREQ_REGISTER_H__
#define __FREQ_REGISTER_H__

#include <systemc.h>
#include "config.h"
#include "globals.h"

SC_MODULE(feeder)
{
  sc_in_clk clock;
  sc_out<sc_uint<FREQSCALING_BITS> > *feed;

  unsigned short int outputs;
  sc_uint<FREQSCALING_BITS> *feed_t;
  
  void simulfeeder();
  void update(unsigned int addr, uint32_t value);
  uint32_t get_value(unsigned int addr); 

  // Constructor
  SC_CTOR(feeder);
  feeder(sc_module_name nm, unsigned short int outputs) :
    sc_module(nm), outputs(outputs)
  {
    feed = new sc_out<sc_uint<FREQSCALING_BITS> > [outputs];
    feed_t = new sc_uint<FREQSCALING_BITS> [outputs];

    // default hardware initialization
    for (int i = 0; i < outputs; i ++)
    {
      // FIXME fragile!!!!!!!
      if (i < N_FREQ_DEVICE)
        feed_t[i] = M_DIVIDER[i];
      else
        feed_t[i] = I_DIVIDER[i - N_FREQ_DEVICE];
    }
    
    for (int i = 0; i < outputs; i ++)
      feed[i].initialize(feed_t[i]);
    
    SC_CTHREAD(simulfeeder,clock.pos());
  }
};

#endif // __FREQ_REGISTER_H__
