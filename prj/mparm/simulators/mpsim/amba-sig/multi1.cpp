///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         multi1.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA address/control bus multiplexer
//
///////////////////////////////////////////////////////////////////////////////

#include "multi1.h"

///////////////////////////////////////////////////////////////////////////////
// accept - Muxes the address and control ports of every master in the system,
//          using the hmaster signal coming from the arbiter as a selector. The
//          outputs are the system-level address and control buses.
//          Recognizes master #31 as a special case and drives the htrans signal
//          to IDLE. This is needed to prevent spurious transactions in the system
//          when no master is actually requiring bus access.
void multi1::accept()
{
  sc_uint<32> addr;
  request varstruct;
  sc_uint<5> master = hmaster.read();

  if(master != 31)
  {
    addr = address[master].read();
    haddr.write(addr);
    varstruct = mast[master].read();
    ctrl_signals.write(varstruct);
  }
  else
  {
    varstruct.htrans = AMBA_SIG_HTRANS_IDLE;
    ctrl_signals.write(varstruct);
  }
}
