///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_sig_trace.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Selects the AMBA AHB (signal) signals to be plotted if VCD is enabled
//
///////////////////////////////////////////////////////////////////////////////

#include "globals.h"
#include "amba_sig_signal.h"
#include "core_signal.h"

void traceAMBAAHBSigSignals(sc_trace_file *tf)
{
  uint16_t i;
  char buffer[20];

  // -------------------
  // System-wide signals
  // -------------------
  // system clock
  sc_trace(tf,ClockGen_1,"ClockGen_1");
  // interconnect clock
  if (FREQSCALING)
  {
    for (i = 0; i < N_BUSES; i ++)
    {
      sprintf(buffer, "interconnect_clock_%d", i);
      sc_trace(tf, interconnect_clock_amba[i], buffer);
      sprintf(buffer, "interconnect_div_%d", i);
      sc_trace(tf, interconnect_div_amba[i], buffer);
    }
  }
  // from arbiter
  sc_trace(tf,hmaster,"hmaster");
  // from multi1
  sc_trace(tf,ctrl_signals,"ctrl_signals");
  sc_trace(tf,haddr,"haddr");
  // from multi2
  sc_trace(tf,hwdataout,"hwdataout");
  // from delayer_hwdata
  sc_trace(tf,delayed_hwdataout,"delayed_hwdataout");
  // from multi3
  sc_trace(tf,hrdata,"hrdata");
  // from slaving
  sc_trace(tf,hready,"hready");

#ifndef WITHOUT_OCP
  for(i=0;i<N_CORES;i++)
  {
    sprintf(buffer,"MCmd%d",i);
    sc_trace(tf,MCmd[i],buffer);
    sprintf(buffer,"MBurst%d",i);
    sc_trace(tf,MBurst[i],buffer);
    sprintf(buffer,"MAddr%d",i);
    sc_trace(tf,MAddr[i],buffer);
    //sprintf(buffer,"MAddrSpace%d",i);
    //sc_trace(tf,MAddrSpace[i],buffer);
    sprintf(buffer,"MData%d",i);
    sc_trace(tf,MData[i],buffer);
    sprintf(buffer,"MByteEn%d",i);
    sc_trace(tf,MByteEn[i],buffer);
    sprintf(buffer,"MDataValid%d",i);
    sc_trace(tf,MDataValid[i],buffer);
    sprintf(buffer,"SCmdAccept%d",i);
    sc_trace(tf,SCmdAccept[i],buffer);
    sprintf(buffer,"SDataAccept%d",i);
    sc_trace(tf,SDataAccept[i],buffer);
    sprintf(buffer,"SResp%d",i);
    sc_trace(tf,SResp[i],buffer);
    sprintf(buffer,"SData%d",i);
    sc_trace(tf,SData[i],buffer);
    sprintf(buffer,"MRespAccept%d",i);
    sc_trace(tf,MRespAccept[i],buffer);
  }
#endif  
  
  // -----------------------
  // Device-specific signals
  // -----------------------
  // How many masters, slaves and DMAs to be traced
#define N_MAST_TRACE N_MASTERS
#define N_SLAVE_TRACE N_SLAVES
#define N_DMA_TRACE N_CORES

  // Master signals
  // --------------

  // request to arbiter
  for(i=0;i<N_MAST_TRACE;i++)
  {
    if (FREQSCALING && (i < N_FREQ_DEVICE))
    {
      sprintf(buffer,"init_clock%d",i);
      sc_trace(tf,init_clock_amba[i],buffer);

      sprintf(buffer,"init_div%d",i);
      sc_trace(tf,init_div_amba[i],buffer);
    }
  
    sprintf(buffer,"hreq%d",i);
    sc_trace(tf,hreq[i],buffer);

    // grant from arbiter
    sprintf(buffer,"hgrant%d",i);
    sc_trace(tf,hgrant[i],buffer);

    // address to multi1
    sprintf(buffer,"address%d",i);
    sc_trace(tf,address[i],buffer);

    // data to multi2
    sprintf(buffer,"hwdata%d",i);
    sc_trace(tf,hwdata[i],buffer);

  /*
    // control signals to multi1
    sprintf(buffer,"mast%d",i);
    sc_trace(tf,mast[i],buffer);
*/
    // ready to wrapper (core frequency)
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
    sc_trace(tf,readymast[i],buffer);

    // pinout to/from wrapper (interconnect frequency)
    sprintf(buffer,"pinoutmast_interconnect%d",i);
    sc_trace(tf,pinoutmast[i],buffer);

    // request from wrapper (interconnect frequency)
    sprintf(buffer,"requestmast_interconnect%d",i);
    sc_trace(tf,requestmast[i],buffer);
  }

  // Slave signals
  // -------------

  // slave select (original) from decoder
  for(i=0;i<N_SLAVE_TRACE;i++)
  {
    sprintf(buffer,"hsel%d",i);
    sc_trace(tf,hsel[i],buffer);

  /*
    // slave select (delayed) from delayer_hsel
    sprintf(buffer,"delayed_hsel%d",i);
    sc_trace(tf,delayed_hsel[i],buffer);
  */

    // ready to slaving
    sprintf(buffer,"ready%d",i);
    sc_trace(tf,ready[i],buffer);

    // data to multi3
    sprintf(buffer,"readdata%d",i);
    sc_trace(tf,readdata[i],buffer);
  }

  /*
  // interrupt signals
  for(i=0;i<N_INTERRUPT*N_CORES;i++)
  {
    sprintf(buffer,"extint%d",i);
    sc_trace(tf,extint[i],buffer);
  }
  */

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
}
