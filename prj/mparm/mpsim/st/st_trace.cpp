///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         st_trace.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Selects the STBus signals to be plotted if VCD is enabled
//
///////////////////////////////////////////////////////////////////////////////


#include "st_trace.h"
#include "snoopdev.h"

void traceSTBusSignals(sc_trace_file *tf)
{
  unsigned int i;
  char buffer[100];

  // -------------------
  // System-wide signals
  // -------------------
  // system clock
  sc_trace(tf, ClockGen_1, "ClockGen_1");
  // interconnect clock
  if (FREQSCALING || FREQSCALINGDEVICE)
  {
    for (i = 0; i < N_BUSES; i ++)
    {
      sprintf(buffer, "interconnect_clock_%d", i);
      sc_trace(tf, interconnect_clock[i], buffer);
      sprintf(buffer, "interconnect_div_%d", i);
      sc_trace(tf, interconnect_div[i], buffer);
    }
  }
  // reset signal (true high)
  sc_trace(tf, ResetGen_1, "ResetGen_1");
  // reset signal (true low)
  sc_trace(tf, ResetGen_low_1, "ResetGen_low_1");

  // -----------------------
  // Device-specific signals
  // -----------------------
  // How many masters, slaves and DMAs to be traced
#define N_MAST_TRACE N_MASTERS+1
#define N_SLAVE_TRACE N_SLAVES
#define N_DMA_TRACE N_CORES

  // ready to wrapper (core frequency)
  for(i=0;i<N_MAST_TRACE;i++)
  {
    if ((FREQSCALING || FREQSCALINGDEVICE) && (i < (unsigned int)(N_FREQ_DEVICE)))
    {
      sprintf(buffer,"init_clock%d",i);
      sc_trace(tf,init_clock[i],buffer);

      sprintf(buffer,"init_div%d",i);
      sc_trace(tf,init_div[i],buffer);
    }
    
    sprintf(buffer,"readymast%d",i);
    sc_trace(tf,readymast[i],buffer);

    // pinout to/from wrapper (core frequency)
    sprintf(buffer,"pinoutmast%d",i);
    sc_trace(tf,pinoutmast[i],buffer);

    // request from wrapper (core frequency)
    sprintf(buffer,"requestmast%d",i);
    sc_trace(tf,requestmast[i],buffer);
  
    // ready to wrapper (interconnect frequency)
    sprintf(buffer,"readymast_interconnect%d",i);
    sc_trace(tf,readymast_interconnect[i],buffer);

    // pinout to/from wrapper (interconnect frequency)
    sprintf(buffer,"pinoutmast_interconnect%d",i);
    sc_trace(tf,pinoutmast_interconnect[i],buffer);

    // request from wrapper (interconnect frequency)
    sprintf(buffer,"requestmast_interconnect%d",i);
    sc_trace(tf,requestmast_interconnect[i],buffer);

    sprintf(buffer,"inittonode%d",i);
    sc_trace(tf,initiator_to_node[i],buffer,0xfffff);
  }
  
    /*
  // interrupt signals
  for(i=0;i<N_INTERRUPT*N_CORES;i++)
  {
    sprintf(buffer,"extint%d",i);
    sc_trace(tf,extint[i],buffer);
  }
  */

  // Slave signals
  // -------------
  for(i=0;i<N_SLAVE_TRACE;i++)
  {
    sprintf(buffer,"nodetotarget%d",i);
    sc_trace(tf,node_to_target[i],buffer,0xfffff);
  }


  if (DMA)
  {
    // DMA signals
    // -----------
    /*
    for(i=0;i<N_DMA_TRACE;i++)
    {
      sprintf(buffer,"requestcontroltotransfer%d",i);
      sc_trace(tf,requestcontroltotransfer[i],buffer);
      
      sprintf(buffer,"finishedtransfer%d",i);
      sc_trace(tf,finishedtransfer[i],buffer);
      
      sprintf(buffer,"requestwrappertodma%d",i);
      sc_trace(tf,requestwrappertodma[i],buffer);
      
      sprintf(buffer,"readywrappertodma%d",i);
      sc_trace(tf,readywrappertodma[i],buffer);

      sprintf(buffer,"datadmacontrol%d",i);
      sc_trace(tf,pinoutwrappertodma[i],buffer);
    }
    */
  }

#ifdef INITIAL_DEBUGGING
     /* finche' e' definita la INITIAL_DEBUGGING in snoopdev.h  */
#ifndef NOSNOOP
  if (SNOOPING)
    for (i=0; i<N_CORES; i++)
      snoop[i]->to_trace(tf);
#endif
#endif

#ifndef NOSNOOP
sc_trace(tf,SnoopSignal_d[0],"SnoopSignal_d[0]",0xfffff);
sc_trace(tf,SnoopSignal_r[0],"SnoopSignal_r[0]",0xfffff);
#endif


#ifdef WITH_POWER_NODE
  if (POWERSTATS)
    pow_set_vcd_trace_file(tf);
#endif
}

