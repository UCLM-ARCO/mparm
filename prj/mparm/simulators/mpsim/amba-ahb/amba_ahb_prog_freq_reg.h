///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_prog_freq_reg.h
// author       DEIS - Universita' di Bologna
//              Martino Ruggiero - mruggiero@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// info         AMBA AHB programmable register for frequency scaling
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_PROG_FREQ_REG_H__
#define __AMBA_AHB_PROG_FREQ_REG_H__

#include <systemc.h>
#include "globals.h"
#include "freq_register.h"
#include "amba_ahb_slave.h"

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_prog_freq_reg - This class inherits an AMBA AHB slave and provides
//                          the functionality of a frequency scaling device,
//                          with an internal register file which holds the
//                          clock dividers for all system components.
class amba_ahb_prog_freq_reg : public amba_ahb_slave
{
  public:
    sc_out<sc_uint<FREQSCALING_BITS> > *p_feed;
    unsigned short int outputs;
    feeder *p_register;

  private:
    inline void Write(uint32_t addr, uint32_t data, uint8_t bw, bool *must_wait)
    {
      // Calculate the target device number
      addr = ((addr - START_ADDRESS) / 4);
      if (addr >= outputs)
      {
        printf("Fatal Error: %s: cannot set divider for device %u!\n", name(), addr);
        exit(1);
      }

      TRACEX(WHICH_TRACEX, 7, "%s: Setting divider %u for device %u at %10.1f ns\n", type, data, addr, sc_simulation_time());
      
      p_register->update(addr, data);
      
      *must_wait = false;
    }

    inline uint32_t Read(uint32_t addr, bool *must_wait)
    {
      // Calculate the target device number
      addr = ((addr - START_ADDRESS) / 4);
      if (addr >= outputs)
      {
        printf("Fatal Error: %s: cannot check divider for device %u!\n", name(), addr);
        exit(1);
      }
      
      TRACEX(WHICH_TRACEX, 8, "%s: Checking divider for device %u at %10.1f ns\n", type, addr, sc_simulation_time());
      
      *must_wait = false;
      
      return p_register->get_value(addr);
    }

  public:
    amba_ahb_prog_freq_reg (sc_module_name nm,
                               uint8_t ID,
                               uint32_t start,
                               uint32_t size, 
                               uint mem_in_ws,
                               uint mem_bb_ws,
                               unsigned short int outputs)
                            : amba_ahb_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws),
                              outputs(outputs)
    {
      p_feed = new sc_out<sc_uint<FREQSCALING_BITS> > [outputs];
      
      type = "amba_ahb_prog_freq_reg";
      WHICH_TRACEX = FREQDEVICE_TRACEX;
      
      // Programmable register for the frequency divider
      p_register = new feeder("p_register", outputs);
      p_register->clock(clock);
      
      for (int i = 0; i < outputs; i ++)
        p_register->feed[i](p_feed[i]);

      TRACEX(WHICH_TRACEX, 7, "%s %d: Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws);
    }
};

#endif // __AMBA_AHB_PROG_FREQ_REG_H__
