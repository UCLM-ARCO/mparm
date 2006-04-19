///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         dual_clock_pinout_slave_if.cpp
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         Implements the slave of a dual-clock sync module, with pinout interface
//
///////////////////////////////////////////////////////////////////////////////

#include "dual_clock_pinout_slave_if.h"

void dual_clock_pinout_slave_if::working()
{
  PINOUT mast_pinout;
  int burst = 0, i;
  bool wr;

  request_to_push.write(false); 
  request_to_pop.write(false); 

  while (true)
  {
    // Wait until someone requests something
    wait_until(request_from_wrapper.delayed() == true);
    // What's up?
    mast_pinout = pinout.read();

    wr = mast_pinout.rw;
    burst = (int)mast_pinout.burst;
    
    // ------------------------------ READ ACCESS ------------------------------   
    if (!wr)
    {
      while (push_full.read() == true)
      {
        TRACEX(DCFIFO_TRACEX, 11, "%s: push_full.read() == true\n", name());
        wait();
      }
 
      pinout_to_wrf.write(mast_pinout);
      request_to_push.write(true);
 
      for (i = 0; i < burst; i ++)
      {
        do
        {
          wait();
          ready_to_wrapper.write(false);
          request_to_push.write(false);
        } while (pop_empty.read() == true);
 
        request_to_pop.write(true);
        wait();
        
        request_to_pop.write(false);
        wait();
 
        mast_pinout = pinout_from_rdf.read();
        pinout.write(mast_pinout);
        ready_to_wrapper.write(true);
      }
 
      wait();
      ready_to_wrapper.write(false);
    }
    // ------------------------------ WRITE ACCESS ------------------------------   
    else
    {
      while (push_full.read() == true)
      {
        TRACEX(DCFIFO_TRACEX, 11, "%s: push_full.read() == true\n", name());
        wait();
      }

      pinout_to_wrf.write(mast_pinout);
      request_to_push.write(true);
      ready_to_wrapper.write(true);
      wait();
 
      ready_to_wrapper.write(false); 
      request_to_push.write(false);
      /*
      do{
      wait();
      ready_to_wrapper.write(false);
      request_to_push.write(false);
      } while(pop_empty.read() == true);
      */
    }
  }
}
