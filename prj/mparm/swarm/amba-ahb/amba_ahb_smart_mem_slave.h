///////////////////////////////////////////////////////////////////////////////
// Copyright 2003 DEIS - Universita' di Bologna
// 
// name         amba_ahb_smart_mem_slave.h
// author       DEIS - Universita' di Bologna
//              Davide Bertozzi - dbertozzi@deis.unibo.it
//              Mirko Loghi - mloghi@deis.unibo.it
//              Federico Angiolini - fangiolini@deis.unibo.it
//              Francesco Poletti - fpoletti@deis.unibo.it
// portions by  Massimo Scardamaglia - mascard@vizzavi.it
// info         AMBA AHB Smart Memory device (slave side)
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __AMBA_AHB_SMART_MEM_SLAVE_H__
#define __AMBA_AHB_SMART_MEM_SLAVE_H__

#include <systemc.h>
#include "globals.h"
#include "ext_mem.h"
#include "address.h"
#include "amba_ahb_slave.h"
#include "power.h"
#include "stats.h"
#include "debug.h"
#include "smartmem.h" //FIXME

///////////////////////////////////////////////////////////////////////////////
// amba_ahb_smart_mem_slave - This class inherits an AMBA AHB slave and provides
//                            the functionality of a smart memory.
class amba_ahb_smart_mem_slave : public amba_ahb_slave
{
  public:
    // ports towards a DMA engine
    sc_inout<PINOUT> pinoutdma;
    sc_in<bool>  readydma;
    sc_out<bool> requestdma;

  private:
    PINOUT pinout;
    // ID of the master port needed to connect to the DMA
    uint8_t ID1;
    Scratch_mem *target_mem;

    inline void Write(uint32_t addr, uint32_t data, uint8_t bw, bool *must_wait)
    {
      double pwr = 0.0;
        
      if ((addr - START_ADDRESS) < SMARTMEM_DMA_SIZE)
      {
        // create the pinout to be sent to the DMA port
        pinout.address = addr;
        pinout.bw      = bw;
        pinout.rw      = 1; // write
        pinout.benable = 1;
        pinout.data    = data;
        // Pass slave pinout to DMA and assert request
        pinoutdma.write(pinout);
        requestdma.write(true);
        
        if (readydma.read() == false)
        {
          *must_wait = true;
          return;
        }
        
        TRACEX(WHICH_TRACEX, 9, "%s %d: Write data 0x%08x to DMA at address\n", type, ID, pinout.data, addr);

        requestdma.write(false); 
          
        if (POWERSTATS)
        {
          pwr = powerDMA(ID1-(!DMA)*N_CORES,SMARTMEM_DMA_SIZE, 32, WRITEop);
          statobject->inspectDMAprogramAccess(false,pwr,ID1-(!DMA)*N_CORES,SMARTMEM_DMA_SIZE);
        }
      }
      else
      {
        TRACEX(WHICH_TRACEX, 9, "%s %d: Write data 0x%08x to smart memory at address 0x%08x\n", type, ID, data, addr);

        if (POWERSTATS)
        {
          pwr = powerRAM(ID, TARGET_MEM_SIZE, 32, READop);
          statobject->inspectSMARTMEMAccess(addr, true, pwr, ID);
        }
        else
          if(STATS)
            statobject->inspectSMARTMEMAccess(addr, true, pwr, ID);

        target_mem->Write(addr, data, bw);
      }
      
      *must_wait = false;
    }

    
    inline uint32_t Read(uint32_t addr, bool *must_wait)
    {
      double pwr = 0.0;
      
      if ((addr - START_ADDRESS) < SMARTMEM_DMA_SIZE)
      {
        // create the pinout to be sent to the DMA port
        pinout.address = addr;
        pinout.bw      = 0; // 32-bit word
        pinout.rw      = 0; // read
        pinout.benable = 1;
        pinout.data    = 0;
        // Pass slave pinout to DMA and assert request
        pinoutdma.write(pinout);
        requestdma.write(true);
        
        if (readydma.read() == false)
        {
          *must_wait = true;
          return 0xdeadbeef;
        }

        // get the data from the dma
        pinout = pinoutdma.read();
        requestdma.write(false);
        
        TRACEX(WHICH_TRACEX, 9, "%s %d: Read data 0x%08x from DMA at address\n", type, ID, pinout.data, addr);

        if (POWERSTATS)
        {
          pwr = powerDMA(ID1-(!DMA)*N_CORES,SMARTMEM_DMA_SIZE, 32, READop);
          statobject->inspectDMAprogramAccess(false, pwr,
                                              ID1-(!DMA)*N_CORES,
                                              SMARTMEM_DMA_SIZE);
        }
        
        *must_wait = false;
        
        return pinout.data;
      }
      else
      {
        uint32_t data;
        
        if (POWERSTATS)
        {
          pwr = powerRAM(ID, TARGET_MEM_SIZE, 32, WRITEop);
          statobject->inspectSMARTMEMAccess(addr, false, pwr, ID);
        }
        
        if (STATS)
          statobject->inspectSMARTMEMAccess(addr, false, pwr, ID); 
        
        data = target_mem->Read(addr);
        
        TRACEX(WHICH_TRACEX, 9, "%s %d: Read data 0x%08x from smart memory at address 0x%08x\n", type, ID, data, addr);
        
        *must_wait = false;
        
        return data;
      }
    }
    
 
  public:
    amba_ahb_smart_mem_slave (sc_module_name nm,
                              uint8_t ID,
                              uint8_t ID1,
                              uint32_t start,
                              uint32_t size,
                              uint mem_in_ws,
                              uint mem_bb_ws,
                              Scratch_mem *target_mem)
                           : amba_ahb_slave(nm, ID, start, size, mem_in_ws, mem_bb_ws),
                             ID1(ID1),
                             target_mem(target_mem)
    {
      type = "amba_ahb_smart_mem_slave";
      WHICH_TRACEX = CORESLAVE_TRACEX;
   
      TRACEX(WHICH_TRACEX, 7, "%s %d: ID1 = %d Start = 0x%08x Size = 0x%08x Wait states = %u\n",
        type, ID, ID1, START_ADDRESS, TARGET_MEM_SIZE, mem_in_ws); 
  }
};

#endif // __AMBA_AHB_SMART_MEM_SLAVE_H__
