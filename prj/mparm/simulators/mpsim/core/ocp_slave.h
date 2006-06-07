///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         ocp_slave.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Generic OCP slave
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OCP_SLAVE_H__
#define __OCP_SLAVE_H__

#include <systemc.h>
#include "globals.h"
#include "address.h"
#include "core_signal.h"
#include "mem_class.h"
#include "power.h"

///////////////////////////////////////////////////////////////////////////////
// ocp_slave - This class encapsulates an OCP slave.
//             nm: slave name
//             ID: slave ID
//             START_ADDRESS: start address of the slave addressing
//                            range (which is here supposed to be unique)
//             TARGET_MEM_SIZE: size of the slave addressing range
//             mem_in_ws: wait states for the initial transfer of a
//                        transaction
//             mem_bb_ws: wait states for back-to-back transfers of
//                        a transaction
SC_MODULE(ocp_slave)
{
  public:
    sc_in_clk                        clock;
    sc_in<bool>                      reset;
    
    // OCP interface
    sc_in<sc_uint<MCMDWD> >          MCmd;
    sc_in<sc_uint<MATOMICLENGTHWD> > MAtomicLength;
    sc_in<sc_uint<MBURSTLENGTHWD> >  MBurstLength;
    sc_in<bool>                      MBurstPrecise;
    sc_in<sc_uint<MBURSTSEQWD> >     MBurstSeq;
    sc_in<bool>                      MBurstSingleReq;
    sc_in<bool>                      MDataLast;
    sc_in<bool>                      MReqLast;
    sc_in<sc_uint<MADDRWD> >         MAddr;
    sc_in<sc_uint<MADDRSPACEWD> >    MAddrSpace;
    sc_in<sc_uint<MDATAWD> >         MData;
    sc_in<sc_uint<MBYTEENWD> >       MByteEn;
    sc_in<bool>                      MDataValid;
    sc_out<bool>                     MRespAccept;
    sc_out<bool>                     SCmdAccept;
    sc_out<bool>                     SDataAccept;
    sc_out<sc_uint<MDATAWD> >        SData;
    sc_out<sc_uint<SRESPWD> >        SResp;
    sc_out<bool >                    SRespLast;
    sc_in<sc_uint<MFLAGWD> >         MFlag;
    sc_out<bool>                     SInterrupt;
    sc_out<sc_uint<SFLAGWD> >        SFlag;
    
  protected:
    unsigned short int               ID;
    char                             *type;
    unsigned long int                START_ADDRESS;
    unsigned long int                TARGET_MEM_SIZE;
    unsigned int                     mem_in_ws;
    unsigned int                     mem_bb_ws;
    unsigned long int                WHICH_TRACEX;
    Mem_class                        *target_mem;

    void receive();
    void issue_interrupts();
    
    inline virtual void inspect_power(int op, uint32_t temp_addr)
    {
      double energy;
      energy = powerRAM(ID, TARGET_MEM_SIZE, 32, op);
      statobject->inspectMemoryAccess(temp_addr, (op == (int)READop) ? 1 : 0, energy, ID);
    }
  
    inline virtual uint32_t addressing(uint32_t addr)
    {
      return addr - START_ADDRESS;
    }
  
    inline virtual void Write(uint32_t addr, uint32_t data, uint8_t bw)
    {
      uint8_t memclass_bw = 3;  //FIXME. Annoying, and no error checking...
      if (bw == OCPMBYEWORD)
        memclass_bw = 0;
      else if (bw == OCPMBYEHWRD)
             memclass_bw = 2;
           else if (bw == OCPMBYEBYTE)
                  memclass_bw = 1;
      
      if (POWERSTATS)
        inspect_power(WRITEop,addr);
    
      addr = addressing(addr);

      target_mem->Write(addr, data, memclass_bw);
    }

    inline virtual uint32_t Read(uint32_t addr)
    {
      if (POWERSTATS)
        inspect_power(READop,addr);
  
      addr = addressing(addr);

      return target_mem->Read(addr);
    }

  SC_HAS_PROCESS(ocp_slave);
    
  public:
    ocp_slave(sc_module_name nm,
              unsigned char ID,
              unsigned long int START_ADDRESS,
              unsigned long int TARGET_MEM_SIZE,
              unsigned short int mem_in_ws,
              unsigned short int mem_bb_ws)
          : sc_module(nm),
            ID(ID),
            START_ADDRESS(START_ADDRESS),
            TARGET_MEM_SIZE(TARGET_MEM_SIZE),
            mem_in_ws(mem_in_ws),
            mem_bb_ws(mem_bb_ws)
    {
      SC_CTHREAD(receive, clock.pos());
      SC_CTHREAD(issue_interrupts, clock.pos());
    }
};

#endif // __OCP_SLAVE_H__
