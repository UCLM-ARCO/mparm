///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         ocp_to_amba_sig_slave.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Links an OCP (slave) interface to an AMBA (signal model) bus
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OCP_TO_AMBA_SIG_SLAVE_H__
#define __OCP_TO_AMBA_SIG_SLAVE_H__

#include <systemc.h>
#include "amba_sig_signal.h"
#include "core_signal.h"

SC_MODULE(ocpambasigslave)
{
  sc_in_clk clock;
  // OCP interface
  sc_in<sc_uint<MCMDWD> >       MCmd;
  sc_in<sc_uint<MBURSTWD> >     MBurst;
  sc_in<sc_uint<MADDRWD> >      MAddr;
  sc_in<sc_uint<MADDRSPACEWD> > MAddrSpace;
  sc_in<sc_uint<MDATAWD> >      MData;
  sc_in<sc_uint<MBYTEENWD> >    MByteEn;
  sc_in<bool>                   MDataValid;
  sc_out<bool>                  MRespAccept;
  sc_out<bool>                  SCmdAccept;
  sc_out<bool>                  SDataAccept;
  sc_out<sc_uint<MDATAWD> >     SData;
  sc_out<sc_uint<SRESPWD> >     SResp;
  //FIXME OCP interrupts
  // AMBA AHB signals
  sc_in<bool> hgrant;
  sc_in<bool> hready;
  sc_in<sc_uint<32> > hrdata;
  sc_out<request> mast;
  sc_out<sc_uint<32> > address;
  sc_out<sc_uint<32> > hwdata;
  sc_out<bool> hreq;

  uint16_t my;

  void working();

  SC_CTOR(ocpambasigslave)
  {
    sscanf(name(), "mst%hu", &my);

    SC_CTHREAD(working, clock.pos());
  }
};

#endif // __OCP_TO_AMBA_SIG_SLAVE_H__
