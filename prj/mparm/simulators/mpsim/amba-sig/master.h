///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         master.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA (signal) bus master
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MASTER_H__
#define __MASTER_H__

#include <systemc.h>
#include "amba_sig_signal.h"
#include "core_signal.h"

SC_MODULE(amba_sig_master)
{
  sc_in_clk clock;
  sc_in<bool> hgrant;
  sc_in<bool> hready;
  sc_in<sc_uint<32> > hrdata;
  sc_out<request> mast;
  sc_out<sc_uint<32> > address;
  sc_out<sc_uint<32> > hwdata;
  sc_out<bool> hreq;
  sc_in<bool> request_from_wrapper;
  sc_out<bool> ready_to_wrapper;
  sc_inout<PINOUT> pinout;

  uint16_t my;

  void working();

  SC_CTOR(amba_sig_master)
  {
    sscanf(name(), "mst%hu", &my);

    SC_CTHREAD(working, clock.pos());
  }
};

#endif // __MASTER_H__
