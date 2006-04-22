///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         reset.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Issues a reset signal at boot
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __RESET_H__
#define __RESET_H__

#include <systemc.h>

SC_MODULE(resetter)
{
  sc_in_clk clock;
  sc_out<bool> reset_out_true_high;
  sc_out<bool> reset_out_true_low;

  void issue();
  
  unsigned long int issuecycles;

  SC_HAS_PROCESS(resetter);
  
  resetter(sc_module_name nm, unsigned long int issuecycles) : sc_module(nm), issuecycles(issuecycles)
  {
    SC_CTHREAD(issue, clock.pos());
    
    reset_out_true_high.initialize(true);
    reset_out_true_low.initialize(false);
  }
};

#endif // __RESET_H__
