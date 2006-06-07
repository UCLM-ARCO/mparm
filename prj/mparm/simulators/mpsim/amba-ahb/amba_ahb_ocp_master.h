///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_ocp_master.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA AHB OCP bus master
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_OCP_MASTER_H__
#define __AMBA_AHB_OCP_MASTER_H__

#include <systemc.h>
#include "amba_ahb_common.h"
#include "amba_ahb_interconnect_if.h"
#include "core_signal.h"
#include "address.h"

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_ocp_master - This class encapsulates an AMBA AHB OCP master.
//                       nm: master name
//                       ID: master ID
SC_MODULE(amba_ahb_ocp_master)
{
    sc_in<bool>                       clock;
    sc_in<bool>                       reset;
    
    // OCP request
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
    sc_in<bool>                       MDataValid;
    sc_in<sc_uint<MBYTEENWD> >        MByteEn;
    sc_out<bool>                      SCmdAccept;
    sc_out<bool>                      SDataAccept;
    
    // OCP response
    sc_out<sc_uint<SRESPWD> >         SResp;
    sc_out<bool>                      SRespLast;
    sc_out<sc_uint<SDATAWD> >         SData;
    sc_in<bool>                       MRespAccept;
    
    // OCP sideband
    sc_in<sc_uint<MFLAGWD> >          MFlag;
    sc_out<bool>                      SInterrupt;
    sc_out<sc_uint<SFLAGWD> >         SFlag;
    
    // #ifndef OLD_INT_STYLE
    sc_inout<bool>                    *extint;
    // #endif
  
    sc_port<ahb_interconnect_if>      master_port;
  
  private:
    // System ID
    unsigned char global_ID;
    // Local ID (on this bus alone)
    unsigned char local_ID;
    // Tracing of transactions for TG use
    FILE                              *ftrace;
    char                              outname[100];
    sc_signal<bool>                   already_written_cmd, already_written_resp;
    char                              buffer_string[4000]; // FIXME pretty unreliable...
    
    // #ifndef OLD_INT_STYLE
    sc_uint<SFLAGWD>                  int_mask;
    // #endif
    
  public:
    void working();
    void tg_trace_collection();
    // #ifndef OLD_INT_STYLE
    void int_forwarding();
    // #endif
  
    SC_HAS_PROCESS(amba_ahb_ocp_master);
  
    amba_ahb_ocp_master(sc_module_name nm, unsigned char ID) : sc_module(nm), global_ID(ID)
    {
      SC_CTHREAD(working, clock.pos());
      
      // #ifndef OLD_INT_STYLE
      extint = new sc_inout<bool> [NUMBER_OF_EXT];
      // #endif
#ifndef OLD_INT_STYLE
      SC_CTHREAD(int_forwarding, clock.pos());
#endif
      
      if (TG_TRACE_COLLECTION)
      {
        sprintf(outname, "trace_%d_%u_0x%08x_0x%08x.trc", ID, CLOCKPERIOD,
          addresser->ReturnSlavePhysicalAddress(addresser->SemaphoreStartID()),
          addresser->ReturnSemaphoreSize(addresser->SemaphoreStartID())); // FIXME assumes only one semaphore will be instantiated
        if (!(ftrace = fopen(outname, "w")))
        {
          fprintf(stderr, "Error opening trace output file %s for writing\n", outname);
          exit(1);
        }
        
        // Spawn a thread specifically to collect TG traces
        SC_CTHREAD(tg_trace_collection, clock.pos());
      }
    }
};

#endif // __AMBA_AHB_OCP_MASTER_H__
