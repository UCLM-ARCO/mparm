///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_sig_target.cpp
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a generic AMBA AHB bus slave
//
///////////////////////////////////////////////////////////////////////////////

#include "amba_sig_target.h"
#include "debug.h"

void amba_sig_slave::receive()
{
  uint32_t address, data, bw=0;
  request varstruct;

  ready0.write (true);
  while(ctrl_signals.read().htrans != AMBA_SIG_HTRANS_NONSEQ)
    wait();
  if (hsel0.read() == false)
  {
    ready0.write (false);
    do
      wait_until(hsel0.delayed() == true && hready.delayed() == true);
    while(ctrl_signals.read().htrans != AMBA_SIG_HTRANS_NONSEQ);
  }
  else
  {
    wait();
    ready0.write (false);
  }

  while (true)
  {
    varstruct = ctrl_signals.read ();
    //address = addresser->Logical2Base(addresser->Physical2Logical(haddr.read(), my));
    address = haddr.read();
    ready0.write (false);

    switch(varstruct.hsize)
      {case 2: 
        bw = MEM_WORD; //dimension of 32 bit
       break;
       case 1:  //half word
        bw = MEM_HWORD;
       break;
       case 0: //byte
        bw = MEM_BYTE;
       break;  
      }
    
    if (varstruct.hwrite == true)  // Writing
    {
      
      if (mem_in_ws)
       wait(mem_in_ws);
  
      data = delayed_hwdataout.read ();
          
      this->Write(address, data, bw);

#ifdef OCCN_DEBUG
      sc_time clk_period; sc_clock *tmp=dynamic_cast<sc_clock*> (clock.get_interface());  
      clk_period = tmp->period();                               
      printf("%s %u: write %8x at address 0x%08x ",type, ID, data, (unsigned int)address);
      cout << " | time: " << (sc_time_stamp()+clk_period) << endl;
#endif

      TRACEX(WHICH_TRACEX, 8,"%s:%d Addr:%x write:%u\n",
       type, ID, (unsigned int)address, data);

    }
    else                         // Reading
    {   
      data = this->Read(address);

      if (mem_in_ws)
        wait(mem_in_ws);

      readdata0.write (data);

#ifdef OCCN_DEBUG       
      printf("%s %u: read  %8x at address 0x%08x ",type, my, data, (unsigned int)address);
      cout << " | time: " << sc_time_stamp() << endl;
#endif
      TRACEX(WHICH_TRACEX, 8,"%s:%d Addr:%x read:%u\n",
       type, ID, (unsigned int)address, data);

    }

    //While no new transaction is requested (and for at least one cycle), keep HREADY high
    ready0.write (true);
    do
      wait();
    while (ctrl_signals.read().htrans != AMBA_SIG_HTRANS_NONSEQ && ctrl_signals.read().htrans != AMBA_SIG_HTRANS_SEQ);
    ready0.write (false);

    //If the new transaction is not targeted at me, wait
    if (hsel0.read() == false)
      do
        wait_until(hsel0.delayed() == true && hready.delayed() == true);
      while(ctrl_signals.read().htrans != AMBA_SIG_HTRANS_NONSEQ);
  };
}
