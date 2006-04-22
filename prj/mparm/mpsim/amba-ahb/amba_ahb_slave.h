///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_slave.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         Describes an AMBA AHB slave.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_SLAVE_H__
#define __AMBA_AHB_SLAVE_H__

#include <systemc.h>
#include "amba_ahb_common.h"
#include "amba_ahb_slave_if.h"
#include "mem_class.h"
#include "address.h"
#include "power.h"

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_slave - This class encapsulates an AMBA AHB slave.
//                  nm: slave name
//                  ID: slave ID
//                  START_ADDRESS: start address of the slave addressing
//                                 range (which is here supposed to be unique)
//                  TARGET_MEM_SIZE: size of the slave addressing range
//                  mem_in_ws: wait states for the initial transfer of a
//                             transaction
//                  mem_bb_ws: wait states for back-to-back transfers of
//                             a transaction (currently unused, since AMBA
//                             AHB slaves don't have a notion of bursts)
class amba_ahb_slave : public ahb_slave_if, public sc_module
{
  public:
    sc_in<bool>                             clock;
    sc_in<bool>                             reset;
  
  protected:
    unsigned int ws_counter;
    unsigned short int bw;
    bool is_write, selected;
    unsigned long int address;

  public:
    void ahb_slave_query(unsigned char IC_ID, AHB_slave_port *port);
    unsigned short int addressing_ranges();
    unsigned long int start_address(unsigned short int range_ID);
    unsigned long int end_address(unsigned short int range_ID);
    
  protected:
    unsigned char ID;
    char *type;
    unsigned long int START_ADDRESS;
    unsigned long int TARGET_MEM_SIZE;
    unsigned int mem_in_ws;
    unsigned int mem_bb_ws;
    unsigned long int WHICH_TRACEX;
    Mem_class *target_mem;
    
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
  
    inline virtual void Write(uint32_t addr, uint32_t data, uint8_t bw, bool *must_wait)
    {
      if (POWERSTATS)
        inspect_power(WRITEop,addr);
    
      addr = addressing(addr);

      target_mem->Write(addr, data, bw);
      
      *must_wait = false;
    }

    inline virtual uint32_t Read(uint32_t addr, bool *must_wait)
    {
      if (POWERSTATS)
        inspect_power(READop,addr);
  
      addr = addressing(addr);
      
      *must_wait = false;

      return target_mem->Read(addr);
    }

    
  public:
    amba_ahb_slave(sc_module_name nm,
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
      ws_counter = 0;
      bw = MEM_WORD;
      selected = false;
    }

};

#endif // __AMBA_AHB_SLAVE_H__
