///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         multi3.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA hrdata bus multiplexer
//
///////////////////////////////////////////////////////////////////////////////

#include "multi3.h"

///////////////////////////////////////////////////////////////////////////////
// accept - Muxes the data out ports of every slave in the system, using the
//          delayed hsel signals coming from the delayer as selectors. The output
//          is the system-level data in bus.
void multi3::accept()
{
  unsigned int data, current = 0;

  for(int i=0; i<N_SLAVES; i++)
    if(delayed_hsel[i].read()==true)
      current = i;
  // Should no delayed_hsel be true, defaults to slave #0

  data=readdata[current].read();
  hrdata.write(data);

#ifdef PRINTDEBUG
  printf("Multi3 read 0x%08x from slave %d at time %10.1f\n", data, current,
           sc_simulation_time());
#endif
}
