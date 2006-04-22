///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         ocp_memory.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a simple synthesizable OCP memory
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OCP_MEMORY_H__
#define __OCP_MEMORY_H__
 
#include <systemc.h>
#include "ocp_signal.h"

#define MEM_SIZE 0x00000c00

typedef enum {
               mem_init,
               mem_read,
               mem_wait_read_mra,
               mem_accept_sr_write,
               mem_wait_sr_write,
               mem_write,
               mem_wrnp,
               mem_wait_wrnp_mra
             } mem_status;

             
SC_MODULE(ocp_memory)
{
  // System clock and reset
  sc_in<bool>                       clock;
  sc_in<bool>                       reset;
  
  // OCP interface
  sc_in<sc_uint<MCMDWD> >           MCmd;
  sc_in<sc_uint<MATOMICLENGTHWD> >  MAtomicLength;
  sc_in<sc_uint<MBURSTLENGTHWD> >   MBurstLength;
  sc_in<bool>                       MBurstPrecise;
  sc_in<sc_uint<MBURSTSEQWD> >      MBurstSeq;
  sc_in<bool>                       MBurstSingleReq;
  sc_in<bool>                       MDataLast;
  sc_in<bool>                       MReqLast;
  sc_in<sc_uint<MADDRWD> >          MAddr;
  sc_in<sc_uint<MADDRSPACEWD> >     MAddrSpace;
  sc_in<sc_uint<MDATAWD> >          MData;
  sc_in<sc_uint<MBYTEENWD> >        MByteEn;
  sc_in<bool>                       MDataValid;
  sc_in<bool>                       MRespAccept;
  sc_out<bool>                      SCmdAccept;
  sc_out<bool>                      SDataAccept;
  sc_out<sc_uint<MDATAWD> >         SData;
  sc_out<sc_uint<SRESPWD> >         SResp;
  sc_out<bool>                      SRespLast;
  sc_in<sc_uint<MFLAGWD> >          MFlag;
  sc_out<bool>                      SInterrupt;
  sc_out<sc_uint<SFLAGWD> >         SFlag;
 
  // Module process
  void receive();
  
  // Internal state
  mem_status st, next_st;
  bool is_last;
  sc_uint<MBURSTLENGTHWD> blen, burst_counter;
  sc_uint<MDATAWD> data;
  sc_uint<MADDRWD> address;
  
  // actual memory
  sc_uint<32> memory[MEM_SIZE / 4];
  
#ifdef XPIPES_SIMULATION
  SC_HAS_PROCESS(ocp_memory);
  ocp_memory::ocp_memory(sc_module_name nm,
                         sc_trace_file *tf,
                         bool tracing) :
    sc_module(nm)
#else
  SC_CTOR(ocp_memory)
#endif
  {
    SC_METHOD (receive);
    sensitive_pos << clock;
    
#ifdef XPIPES_SIMULATION
    if (tracing)
    {
      char namebuffer[100];
      sprintf(namebuffer, "%s_burst_counter", name());
      sc_trace(tf, burst_counter, namebuffer);
      sprintf(namebuffer, "%s_blen", name());
      sc_trace(tf, blen, namebuffer);
      sprintf(namebuffer, "%s_address", name());
      sc_trace(tf, address, namebuffer);
      sprintf(namebuffer, "%s_data", name());
      sc_trace(tf, data, namebuffer);
      sprintf(namebuffer, "%s_is_last", name());
      sc_trace(tf, is_last, namebuffer);
    }
#endif
  }
};

#endif // __OCP_TESTER_H__
