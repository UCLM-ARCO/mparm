///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         decoder.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Address decoder for an AMBA bus
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DECODER_H__
#define __DECODER_H__

#include <systemc.h>
#include "globals.h"

SC_MODULE(decoder)
{
  sc_in_clk clock;
  sc_in<sc_uint<32> > haddr;
  sc_out<bool> *hsel;

  void dec_sel();

  SC_CTOR(decoder)
  {
    hsel = new sc_out<bool> [N_SLAVES];
    SC_METHOD(dec_sel);
    sensitive << haddr;
  }
};

#endif // __DECODER_H__
