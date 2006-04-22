///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_occn_trace.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Selects the AMBA AHB (OCCN) signals to be plotted if VCD is enabled
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_occn_trace.h"

void traceAMBAAHBOCCNSignals(sc_trace_file *tf)
{
  uint16_t i;
  char buffer[20];

  // -------------------
  // System-wide signals
  // -------------------
  // system clock
  sc_trace(tf,ClockGen_1.signal(),"ClockGen_1");

  // -----------------------
  // Device-specific signals
  // -----------------------
  // How many masters, slaves and DMAs to be traced
#define N_MAST_TRACE N_MASTERS
#define N_SLAVE_TRACE N_SLAVES
#define N_DMA_TRACE N_CORES

  // Master signals
  // --------------

  // ready to wrapper
  for(i=0;i<N_MAST_TRACE;i++)
  {
    sprintf(buffer,"readymast%d",i);
    sc_trace(tf,readymast[i],buffer);
  }

  // pinout to/from wrapper
  for(i=0;i<N_MAST_TRACE;i++)
  {
    sprintf(buffer,"pinoutmast%d",i);
    sc_trace(tf,pinoutmast[i],buffer);
  }

  // request from wrapper
  for(i=0;i<N_MAST_TRACE;i++)
  {
    sprintf(buffer,"requestmast%d",i);
    sc_trace(tf,requestmast[i],buffer);
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
    }

    for(i=0;i<N_DMA_TRACE;i++)
    {
      sprintf(buffer,"finishedtransfer%d",i);
      sc_trace(tf,finishedtransfer[i],buffer);
    }

    for(i=0;i<N_DMA_TRACE;i++)
    {
      sprintf(buffer,"requestwrappertodma%d",i);
      sc_trace(tf,requestwrappertodma[i],buffer);
    }

    for(i=0;i<N_DMA_TRACE;i++)
    {
      sprintf(buffer,"readywrappertodma%d",i);
      sc_trace(tf,readywrappertodma[i],buffer);
    }
  
    for(i=0;i<N_DMA_TRACE;i++)
    {
      sprintf(buffer,"datadmacontrol%d",i);
      sc_trace(tf,pinoutwrappertodma[i],buffer);
    }
    */
  }

}
