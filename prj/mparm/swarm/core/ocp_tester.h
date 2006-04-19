///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         ocp_tester.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a simple synthesizable OCP traffic generator
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OCP_TESTER_H__
#define __OCP_TESTER_H__
 
#include <systemc.h>
#include "ocp_signal.h"

#ifdef XPIPES_SIMULATION
  #include "transaction_gen.h"
#else
  #define NUM_TRANSACTIONS 10
#endif

typedef enum {
               tester_init,
               tester_wait_SCA,
               tester_wait_last_SCA,
               tester_wait_SR,
               tester_idle
             } tester_status;

             
SC_MODULE(ocp_tester)
{
  // System clock and reset
  sc_in<bool>                       clock;
  sc_in<bool>                       reset;
  
  // ID line from outside (to be able to differentiate traffic patterns in each ocp_tester
  // when synthesizing. In this case, we can't query external modules to know what
  // traffic to generate)
  sc_in<sc_uint<8> >             id; // FIXME no fixed size
  
  // Output line to check test results
  sc_out<bool>                      test_ok;
  
  // OCP interface
  sc_out<sc_uint<MCMDWD> >          MCmd;
  sc_out<sc_uint<MATOMICLENGTHWD> > MAtomicLength;
  sc_out<sc_uint<MBURSTLENGTHWD> >  MBurstLength;
  sc_out<bool>                      MBurstPrecise;
  sc_out<sc_uint<MBURSTSEQWD> >     MBurstSeq;
  sc_out<bool>                      MBurstSingleReq;
  sc_out<bool>                      MDataLast;
  sc_out<bool>                      MReqLast;
  sc_out<sc_uint<MADDRWD> >         MAddr;
  sc_out<sc_uint<MADDRSPACEWD> >    MAddrSpace;
  sc_out<sc_uint<MDATAWD> >         MData;
  sc_out<sc_uint<MBYTEENWD> >       MByteEn;
  sc_out<bool>                      MDataValid;
  sc_out<bool>                      MRespAccept;
  sc_in<bool>                       SCmdAccept;
  sc_in<bool>                       SDataAccept;
  sc_in<sc_uint<MDATAWD> >          SData;
  sc_in<sc_uint<SRESPWD> >          SResp;
  sc_in<bool>                       SRespLast;
  sc_out<sc_uint<MFLAGWD> >         MFlag;
  sc_in<bool>                       SInterrupt;
  sc_in<sc_uint<SFLAGWD> >          SFlag;

  // Module process
  void request();
  
  // Internal state
  tester_status st, next_st;
  sc_uint<16> burst_counter, wait_counter, trans_counter;
  
#ifdef XPIPES_SIMULATION
  unsigned short int ID;
  transaction_gen *transgen;
  uint32_t data;
  uint32_t address;
  uint burst;
  int idlewait;
  TGEN_BENABLE benable;
  bool wr;
  bool statistics_started, statistics_finished;
#else
  // Memory holding the transactions to reproduce
  sc_uint<32> address[NUM_TRANSACTIONS], data[NUM_TRANSACTIONS];
  sc_uint<16> burst[NUM_TRANSACTIONS], idlewait[NUM_TRANSACTIONS];
  bool wr[NUM_TRANSACTIONS];
#endif
  
#ifdef XPIPES_SIMULATION
  SC_HAS_PROCESS(ocp_tester);
  ocp_tester::ocp_tester(sc_module_name nm,
                         unsigned short int ID,
                         sc_trace_file *tf,
                         bool tracing) :
    sc_module(nm), ID(ID)
#else
  SC_CTOR(ocp_tester)
#endif
  {
    SC_METHOD (request);
    sensitive_pos << clock;
    
#ifdef XPIPES_SIMULATION
    transgen = new transaction_gen(ID, TGEN_TABLE);
    statistics_started = false;
    statistics_finished = false;
      
    if (tracing)
    {
      char namebuffer[100];
      sprintf(namebuffer, "%s_burst_counter", name());
      sc_trace(tf, burst_counter, namebuffer);
      sprintf(namebuffer, "%s_wait_counter", name());
      sc_trace(tf, wait_counter, namebuffer);
      sprintf(namebuffer, "%s_trans_counter", name());
      sc_trace(tf, trans_counter, namebuffer);
      sprintf(namebuffer, "%s_test_ok", name());
      sc_trace(tf, test_ok, namebuffer);
    }
#endif
  }
};

#endif // __OCP_TESTER_H__
