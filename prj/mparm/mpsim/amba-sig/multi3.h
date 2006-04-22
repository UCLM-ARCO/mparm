///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
//
// name         multi3.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA hrdata bus multiplexer
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MULTI3_H__
#define __MULTI3_H__

#include <systemc.h>
#include "globals.h"

SC_MODULE(multi3)
{
  sc_in<bool> *delayed_hsel;
  sc_in<sc_uint<32> > *readdata;
  sc_out<sc_uint<32> > hrdata;
   
  void accept();

  SC_CTOR(multi3)
  {
    delayed_hsel = new sc_in<bool> [N_SLAVES];
    readdata = new sc_in<sc_uint<32> > [N_SLAVES];
    SC_METHOD(accept);
    for (int i=0; i<N_SLAVES; i++)
    {
      sensitive << readdata[i];
      sensitive << delayed_hsel[i];
    }
  }
};

#endif // __MULTI3_H__
