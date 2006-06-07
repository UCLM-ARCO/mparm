///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         dual_clock_pinout_master_if.cpp
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements the master of a dual-clock sync module, with pinout interface
//
///////////////////////////////////////////////////////////////////////////////

#include "dual_clock_pinout_master_if.h"

void dual_clock_pinout_master_if::working()
{
  PINOUT mast_pinout;
  int burst = 0, i;
  bool wr;

  request_to_push.write(false); 
  request_to_pop.write(false); 

  while (true)
  {
    // Wait until someone requests something
    wait_until(pop_empty.delayed() == false);
    
    request_to_pop.write(true);
    wait();
    request_to_pop.write(false);
    wait();
    
    // What's up?
    mast_pinout = pinout_from_wrf.read();

    wr = mast_pinout.rw;
    burst = (int)mast_pinout.burst;
    
    // ------------------------------ READ ACCESS ------------------------------   
    if (!wr)
    {
      pinout.write(mast_pinout);
      request_to_master.write(true);   
      for (i = 0; i < burst; i ++)
      {
        do
        {
          wait();
          request_to_push.write(false);
        } while (!ready_from_master.read());

        mast_pinout = pinout.read();
        pinout_to_rdf.write(mast_pinout);
        request_to_push.write(true);
      }
      
      request_to_master.write(false);  
      wait();
      request_to_push.write(false);
    }
    // ------------------------------ WRITE ACCESS ------------------------------   
    else
    {
      /*
      wait_until(pop_empty.delayed() == false);
      request_to_pop.write(true);
      wait();
      request_to_pop.write(false);
      wait();
      mast_pinout = pinout_from_wrf.read();
      */
      pinout.write(mast_pinout);
      request_to_master.write(true);
      
      wait_until(ready_from_master.delayed() == true);

      request_to_master.write(false);
    }
  }
}
