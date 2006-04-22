///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         multi1.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA address/control bus multiplexer
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MULTI1_H__
#define __MULTI1_H__

#include <systemc.h>
#include "globals.h"
#include "amba_sig_signal.h"

SC_MODULE(multi1)
{
  sc_in_clk clock;
  sc_in<request> *mast;
  sc_in<sc_uint<32> > *address;
  sc_in<sc_uint<5> > hmaster;
  sc_out<sc_uint<32> > haddr;
  sc_out<request> ctrl_signals;

  void accept();

  SC_CTOR(multi1)
  {
    mast = new sc_in<request> [N_MASTERS];
    address = new sc_in<sc_uint<32> > [N_MASTERS];
    SC_METHOD(accept);
    sensitive << hmaster;
    for (int j=0; j<N_MASTERS; j++)
    {
      sensitive << address[j];
      sensitive << mast[j];
    }
 }
};

#endif // __MULTI1_H__
