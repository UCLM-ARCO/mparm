///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         arbiter.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Arbiter for an AMBA bus
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ARBITER_H__
#define __ARBITER_H__

#include <systemc.h>
#include "globals.h"
#include "amba_sig_signal.h"

SC_MODULE(arbiter)
{
  sc_in_clk clock;
  sc_in<bool> *hreq;
  sc_in<bool> hready;
  sc_in<request> ctrl_signals;
  sc_out<sc_uint<5> > hmaster;
  sc_out<bool> *hgrant;

  void arbitering();
  int granting();

  SC_CTOR(arbiter)
  {
    hreq = new sc_in<bool> [N_MASTERS];
    hgrant = new sc_out<bool> [N_MASTERS];
    SC_CTHREAD(arbitering, clock.pos());
  }
};

#endif // __ARBITER_H__
