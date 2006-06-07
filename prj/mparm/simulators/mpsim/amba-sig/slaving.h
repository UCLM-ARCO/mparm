///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         slaving.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Logically ORs ready signals from slaves
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __SLAVING_H__
#define __SLAVING_H__

#include <systemc.h>
#include "globals.h"

SC_MODULE(slaving)
{
  sc_in_clk clock;
  sc_in<bool> *ready;
  sc_in<bool> *hsel;
  sc_out<bool> hready;
  
  void logicalor();

  SC_CTOR(slaving)
  {
    ready = new sc_in<bool> [N_SLAVES];
    hsel = new sc_in<bool> [N_SLAVES];
    SC_METHOD(logicalor);
    for (int i=0; i<N_SLAVES; i++) 
      sensitive << ready[i];
  };
};

#endif // __SLAVING_H__
