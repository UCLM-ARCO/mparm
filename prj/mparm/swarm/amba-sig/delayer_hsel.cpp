///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         delayer_hsel.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Signal delayer for an AMBA bus
//
///////////////////////////////////////////////////////////////////////////////

#include "delayer_hsel.h"

///////////////////////////////////////////////////////////////////////////////
// delay - Generates delayed versions of the hsel signals for use by the slaves.
void delayer_hsel::delay()
{
  unsigned int i, prev = 0;

  for (i=0; i<N_SLAVES; i++)
    delayed_hsel[i].write(false);

  while(true)
  {
    wait_until(hready.delayed() == true);

    for(i=0; i<N_SLAVES; i++)
      if(hsel[i].read() == true && i != prev)
      {
        delayed_hsel[prev].write(false);
        delayed_hsel[i].write(true);
        prev = i;
      }
  }
}
