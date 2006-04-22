///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         delayer_hwdata.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Signal delayer for an AMBA bus
//
///////////////////////////////////////////////////////////////////////////////

#include "delayer_hwdata.h"

///////////////////////////////////////////////////////////////////////////////
// delay - Generates a delayed version of the hwdata bus for use by the slaves.
void delayer_hwdata::delay()
{ 
  while(true)
  {
    delayed_hwdataout.write(hwdataout.read());
    wait_until(hready.delayed() == true);
  }
}
