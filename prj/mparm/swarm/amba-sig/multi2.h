///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         multi2.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA hwdata bus multiplexer
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MULTI2_H__
#define __MULTI2_H__

#include <systemc.h>
#include "globals.h"

SC_MODULE(multi2)
{
  sc_in_clk clock;
  sc_in<sc_uint<32> > *hwdata;
  sc_in<sc_uint<5> > hmaster;
  sc_out<sc_uint<32> > hwdataout;

  void accept();

  SC_CTOR(multi2)
  {
    hwdata = new sc_in<sc_uint<32> > [N_MASTERS];
    SC_METHOD(accept);
    for (int i=0; i<N_MASTERS; i++)
      sensitive << hwdata[i];
    sensitive << hmaster;    
  }
};

#endif // __MULTI2_H__
