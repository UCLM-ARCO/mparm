///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_sig_target.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Implements a generic AMBA AHB bus slave
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_SIG_SLAVE_H__
#define __AMBA_SIG_SLAVE_H__

#include <systemc.h>
#include "globals.h"
#include "mem_class.h"
#include "address.h"
#include "amba_sig_signal.h"
#include "power.h"

SC_MODULE(amba_sig_slave)
{
 
 public:
  
  sc_in_clk clock;
  sc_in<sc_uint<32> > haddr;
  sc_in<sc_uint<32> > delayed_hwdataout;
  sc_in<bool> hsel0;
  sc_in<bool> hready;
  sc_in<request> ctrl_signals;
  sc_out<bool> ready0;
  sc_out<sc_uint<32> > readdata0;

  SC_HAS_PROCESS(amba_sig_slave);
  
  amba_sig_slave(sc_module_name nm, uint8_t id, uint32_t start, uint32_t siz, uint mem_in_ws1)
  {
    ID=id;
    mem_in_ws=mem_in_ws1;
    TARGET_MEM_SIZE=siz;
    START_ADDRESS = start;
    WHICH_TRACEX = TARGET_TRACEX;

    SC_CTHREAD(receive,clock.pos());
  }
    
 protected: 
  
  uint16_t ID;
  
  char* type;
  uint mem_in_ws;
  uint32_t START_ADDRESS;
  uint32_t TARGET_MEM_SIZE;
  uint32_t WHICH_TRACEX;
   
  Mem_class* target_mem;
   
  void receive();
  
  virtual void inspect_power(int op, uint32_t temp_addr)
  {
   double pwr;
   pwr = powerRAM(ID, TARGET_MEM_SIZE, 32, op);
   statobject->inspectMemoryAccess(temp_addr, (op==(int)READop)?1:0, pwr, ID);
  }
  
  virtual uint32_t addressing(uint32_t addr)
  {
   return addr-START_ADDRESS;
  }
  
  virtual void Write(uint32_t addr, uint32_t data, uint8_t bw)
  {
    if (POWERSTATS)
       inspect_power(WRITEop,addr);
    
    addr=addressing(addr);
        	
    target_mem->Write(addr, data, bw);
  }

  virtual uint32_t Read(uint32_t addr)
  {
    if (POWERSTATS)
       inspect_power(READop,addr);
  
    addr=addressing(addr);

    return target_mem->Read(addr);
  }   
};

#endif // __AMBA_SIG_SLAVE_H__
