///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         multi2.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA hwdata bus multiplexer
//
///////////////////////////////////////////////////////////////////////////////

#include "multi2.h"

///////////////////////////////////////////////////////////////////////////////
// accept - Muxes the data out ports of every master in the system, using the
//          hmaster signal coming from the arbiter as a selector. The output is
//          the system-level data out bus.
//          Recognizes master #31 as a special case and does nothing. This is
//          needed to prevent spurious transactions in the system when no master
//          is actually requiring bus access.
void multi2::accept()
{
  sc_uint<5> selector = hmaster.read();
  if (selector != 31)
    hwdataout.write(hwdata[selector].read());
}          
