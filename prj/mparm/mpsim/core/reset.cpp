///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         reset.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Issues a reset signal at boot
//
///////////////////////////////////////////////////////////////////////////////

#include "reset.h"

///////////////////////////////////////////////////////////////////////////////
// issue - Generates a reset signal lasting the first cycle of a simulation.
void resetter::issue()
{
  while(true)
  {
    if (issuecycles == 0)
    {
      reset_out_true_high.write(false);
      reset_out_true_low.write(true);
    }
    else
      issuecycles--;
    wait();
  }
}
